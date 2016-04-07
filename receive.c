
#include "receive.h"
#include "receive_public.h"
#include "debug.h"
#include "command_types.h"

// Instance of struct
RCV_DATA rcvData;

// Initialization function
void RECEIVE_Initialize ( void )
{
    // Create queue, 80 slots, each one byte in size
    rcvData.rcvQueue = xQueueCreate(80, sizeof(char));
    // Verifies queue created
    if (rcvData.rcvQueue == 0)
        error('c');
    
    // Index in current message
    rcvData.currentIndex = 0;
    
    // Initialize timer and uart
    timer_initialize(100);
    DRV_USART0_Initialize();
    
    rcvData.clock = 0;
    rcvData.sequence_out = 100;
    
    rcvData.loc_ready = false;
    rcvData.mov_ready = false;
    rcvData.config_ready = false;
    rcvData.obs_ready = false;
    rcvData.started = false;
    
    //rcvData.xGoal = 0;
    //rcvData.yGoal = 0;
    
    rcvData.turning = false;
    
    // Index in way-point array
    rcvData.index = 0;
    rcvData.build_index = 0;
    rcvData.done = false;
    
    int i, j;
    
    for (i = 0; i < 60; i++)
	{
		rcvData.x_wp[i] = 0;
		rcvData.y_wp[i] = 0;
	}
    
    for (i=0;i<3;i++){
        rcvData.obs_x[i] = 6;
        rcvData.obs_y[i] = 6;
        rcvData.token_retrieved[i] = false;
    }
    
    for (i=0;i<6;i++)
        for (j=0;j<6;j++)
            rcvData.board[i][j] = '0';
    
    rcvData.token_index = 0;
    rcvData.retrieved_index = 0;
    rcvData.timer_threshold = 0;
    
    rcvData.numToks = 1;
}

// Adds character to received queue
int addQRcv(char letter)
{
    if (xQueueSendFromISR (rcvData.rcvQueue, &letter, 0) != pdTRUE)
        error('b');
}

// Updates clock
void updateClock()
{
    rcvData.clock++;
}

void goForward(uint8_t x)
{
    rcvData.turning = false;
    rcvData.sequence_out++;
    addQSnd(START);
    addQSnd(0x70);
    addQSnd(rcvData.sequence_out/256);
    addQSnd(rcvData.sequence_out%256);
    addQSnd(x);
    addQSnd(0x00);
    addQSnd(0x00);
    addQSnd(END);
}

void turn(uint8_t degree)
{
    // initiates a one second wait on each turn to gather position info
    rcvData.timer_threshold = rcvData.clock+10;
    rcvData.turning = true;
    rcvData.sequence_out++;
    addQSnd(START);
    addQSnd(0x71);
    addQSnd(rcvData.sequence_out/256);
    addQSnd(rcvData.sequence_out%256);
    addQSnd(0x00);
    addQSnd(0x00);
    addQSnd(degree);
    addQSnd(END);
}

void stop()
{
    rcvData.turning = false;
    rcvData.sequence_out++;
    addQSnd(START);
    addQSnd(0x72);
    addQSnd(rcvData.sequence_out/256);
    addQSnd(rcvData.sequence_out%256);
    addQSnd(0x00);
    addQSnd(0x00);
    addQSnd(0x00);
    addQSnd(END);
}

void token()
{
    rcvData.turning = false;
    rcvData.sequence_out++;
    addQSnd(START);
    addQSnd(0x76);
    addQSnd(rcvData.sequence_out/256);
    addQSnd(rcvData.sequence_out%256);
    addQSnd(rcvData.square_x);
    addQSnd(rcvData.square_y);
    addQSnd(rcvData.token_index);
    addQSnd(END);
}

void stopAll()
{
    int min = rcvData.clock/600;
    int sec = (rcvData.clock/10)%60;
    rcvData.turning = false;
    rcvData.sequence_out++;
    addQSnd(START);
    addQSnd(0x73);
    addQSnd(rcvData.sequence_out/256);
    addQSnd(rcvData.sequence_out%256);
    addQSnd(min);
    addQSnd(sec);
    addQSnd(0x00);
    addQSnd(END);
}


bool isObs(int x, int y)
{
	int i;
	for (i = 0; i < rcvData.numObs; i++)
	{
		// Checks all obstacles against location
		if (rcvData.obs_square_x[i] == x && rcvData.obs_square_y[i] == y)
			return true;
		// Checks that location is within grid
		if (x < 0 || x > 5 || y < 0 || y > 5)
			return true;
	}
	return false;
}

void addWaypoint(int x, int y)
{
	rcvData.x_wp[rcvData.build_index] = x;
	rcvData.y_wp[rcvData.build_index] = y;
	rcvData.build_index++;
}

void setPoints()
{
	// if looking down, go down. Else go up
	if (rcvData.orient > 70 && rcvData.orient < 110)
		rcvData.up = false;
	else
		rcvData.up = true;
    
	// Assigns temporary variables
	rcvData.tempx = rcvData.square_x;
	rcvData.tempy = rcvData.square_y;

	// Starts by going left
	rcvData.firstPass = true;

	// Allows for changing direction before barrier
	bool turnAround = false;

	// Adds initial point as way-point
	addWaypoint(rcvData.tempx, rcvData.tempy);

	// While haven't completed two passes (right then left)
	while (rcvData.tempx >= 0) {
		// If going up
		if (rcvData.up) { 
			// While not yet at top wall
			while (rcvData.tempy > 0) {
				// Check for obstacle above
				if (isObs(rcvData.tempx, rcvData.tempy - 1)) {
					// Checks that there is room to go around obstacle above
					if (isObs(rcvData.tempx, rcvData.tempy - 2)) {
						// Up against wall
					}
					// Checks if clear to go around left, prefers left on first pass
					else if (!isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy - 1) && !isObs(rcvData.tempx - 1, rcvData.tempy - 2) && rcvData.firstPass) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx-1, rcvData.tempy);
						addWaypoint(rcvData.tempx-1, rcvData.tempy-2);
						addWaypoint(rcvData.tempx, rcvData.tempy-2);
						rcvData.tempy-=2;
					}
					// Checks if clear to go around right, prefers right on second pass
					else if (!isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy - 1) && !isObs(rcvData.tempx + 1, rcvData.tempy - 2) && !rcvData.firstPass) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy - 2);
						addWaypoint(rcvData.tempx, rcvData.tempy - 2);
						rcvData.tempy -= 2;
					}
					// Goes left on second pass if needed
					else if (!isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy - 1) && !isObs(rcvData.tempx - 1, rcvData.tempy - 2)) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy - 2);
						addWaypoint(rcvData.tempx, rcvData.tempy - 2);
						rcvData.tempy -= 2;
					}
					// Goes right on first pass if needed
					else if (!isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy - 1) && !isObs(rcvData.tempx + 1, rcvData.tempy - 2)) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy - 2);
						addWaypoint(rcvData.tempx, rcvData.tempy - 2);
						rcvData.tempy -= 2;
					}
				}
				// no obstacle above
				else
					rcvData.tempy--;
			}
			// add waypoint to complete col
			addWaypoint(rcvData.tempx, rcvData.tempy);
		}
		// If going down
		else {
			// While not yet at bottom wall
			while (rcvData.tempy < 5) {
				// Check for obstacle above
				if (isObs(rcvData.tempx, rcvData.tempy + 1)) {
					// Checks that there is room to go around obstacle below
					if (isObs(rcvData.tempx, rcvData.tempy + 2)) {
						// Up against wall
					}
					// Checks if clear to go around left, prefers left on first pass
					else if (!isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy + 1) && !isObs(rcvData.tempx - 1, rcvData.tempy + 2) && rcvData.firstPass) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy + 2);
						addWaypoint(rcvData.tempx, rcvData.tempy + 2);
						rcvData.tempy += 2;
					}
					// Checks if clear to go around right, prefers right on second pass
					else if (!isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy + 1) && !isObs(rcvData.tempx + 1, rcvData.tempy + 2) && !rcvData.firstPass) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy + 2);
						addWaypoint(rcvData.tempx, rcvData.tempy + 2);
						rcvData.tempy += 2;
					}
					// Goes left on second pass if needed
					else if (!isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy + 1) && !isObs(rcvData.tempx - 1, rcvData.tempy + 2)) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy);
						addWaypoint(rcvData.tempx - 1, rcvData.tempy + 2);
						addWaypoint(rcvData.tempx, rcvData.tempy + 2);
						rcvData.tempy += 2;
					}
					// Goes right on first pass if needed
					else if (!isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy + 1) && !isObs(rcvData.tempx + 1, rcvData.tempy + 2)) {
						addWaypoint(rcvData.tempx, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy);
						addWaypoint(rcvData.tempx + 1, rcvData.tempy + 2);
						addWaypoint(rcvData.tempx, rcvData.tempy + 2);
						rcvData.tempy += 2;
					}
				}
				// no obstacle below
				else 
					rcvData.tempy++;
			}
			// add waypoint to complete row
			addWaypoint(rcvData.tempx, rcvData.tempy);
		}
		if (rcvData.tempx < 5 && rcvData.firstPass) {
			// check if next spot over has obstacle
			if (isObs(rcvData.tempx + 1, rcvData.tempy)) {
				// If going up
				if (rcvData.up && !isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy + 1) && !isObs(rcvData.tempx, rcvData.tempy + 1) && !isObs(rcvData.tempx + 1, rcvData.tempy + 1)) {
					addWaypoint(rcvData.tempx, rcvData.tempy);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy + 1);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy + 1);
					rcvData.tempy ++;
				}
				// If going down
				else if (!rcvData.up && !isObs(rcvData.tempx - 1, rcvData.tempy) && !isObs(rcvData.tempx - 1, rcvData.tempy - 1) && !isObs(rcvData.tempx, rcvData.tempy - 1) && !isObs(rcvData.tempx + 1, rcvData.tempy - 1)) {
					addWaypoint(rcvData.tempx, rcvData.tempy);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy - 1);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy - 1);
					rcvData.tempy--;
				}
			}
			rcvData.tempx++;
			rcvData.up = !rcvData.up;
		}
		else {
			// check if next spot over has obstacle
			if (isObs(rcvData.tempx - 1, rcvData.tempy)) {
				// If going up
				if (rcvData.up && !isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy + 1) && !isObs(rcvData.tempx, rcvData.tempy + 1) && !isObs(rcvData.tempx - 1, rcvData.tempy + 1)) {
					addWaypoint(rcvData.tempx, rcvData.tempy);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy + 1);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy + 1);
					rcvData.tempy++;
				}
				// If going down
				else if (!rcvData.up && !isObs(rcvData.tempx + 1, rcvData.tempy) && !isObs(rcvData.tempx + 1, rcvData.tempy - 1) && !isObs(rcvData.tempx, rcvData.tempy - 1) && !isObs(rcvData.tempx - 1, rcvData.tempy - 1)) {
					addWaypoint(rcvData.tempx, rcvData.tempy);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy);
					addWaypoint(rcvData.tempx + 1, rcvData.tempy - 1);
					addWaypoint(rcvData.tempx - 1, rcvData.tempy - 1);
					rcvData.tempy--;
				}
			}
			rcvData.firstPass = false;
			rcvData.tempx--;
			rcvData.up = !rcvData.up;
		}
		if (rcvData.tempx > -1)
			addWaypoint(rcvData.tempx, rcvData.y_wp[rcvData.build_index - 1]);
	}
}


void maintainLine()
{
    // going down
    if (rcvData.y_wp[rcvData.index] > rcvData.square_y)
    {
        if ((rcvData.xPos > rcvData.x_wp[rcvData.index]*30+20) && (rcvData.orient >= 90) && !rcvData.turning)
            stop();
        else if ((rcvData.xPos < rcvData.x_wp[rcvData.index]*30+10) && (rcvData.orient <= 90) && !rcvData.turning)
            stop();
    }
    // going up
    else if (rcvData.y_wp[rcvData.index] < rcvData.square_y)
    {
        if ((rcvData.xPos > rcvData.x_wp[rcvData.index]*30+20) && (rcvData.orient > 178) && !rcvData.turning)
            stop();
        else if ((rcvData.xPos < rcvData.x_wp[rcvData.index]*30+10) && (rcvData.orient < 2) && !rcvData.turning)
            stop();
    }
}

void goToGoal()
{
    // going down
    if (rcvData.y_wp[rcvData.index]*30+10 > rcvData.yPos)
    {
        // need to turn
        if ((rcvData.orient < 85 || rcvData.orient > 95) && !rcvData.turning)
        {
            turn ((270 - rcvData.orient)%180);
        }
        // off center, and pointing moreso **optional code**
        else if ((rcvData.xPos > rcvData.x_wp[rcvData.index]*30+20) && (rcvData.orient > 92) && !rcvData.turning)
        {
            turn(177);
        }
        // **optional code**
        else if ((rcvData.xPos < rcvData.x_wp[rcvData.index]*30+10) && (rcvData.orient < 88) && !rcvData.turning)
        {
            turn(3);
        }
        // already going down
        else
        {
            rcvData.turning = false;
            goForward(rcvData.y_wp[rcvData.index]*30+15 - rcvData.yPos);
        }
    }
    // going up
    else if (rcvData.y_wp[rcvData.index]*30+20 < rcvData.yPos)
    {
        // need to turn
        if ((rcvData.orient < 175 && rcvData.orient > 5) && !rcvData.turning)
        {
            turn ((180 - rcvData.orient)%180);
        }
        // off center, and pointing moreso
        else if ((rcvData.xPos > rcvData.x_wp[rcvData.index]*30+20) && (rcvData.orient > 178) && !rcvData.turning)
        {
            turn(3);
        }
        // **optional code**
        else if ((rcvData.xPos < rcvData.x_wp[rcvData.index]*30+10) && (rcvData.orient < 2) && !rcvData.turning)
        {
            turn(177);
        }
        // already going up
        else
        {
            rcvData.turning = false;
            goForward(rcvData.yPos - rcvData.y_wp[rcvData.index]*30-15);
        }
    }
    // going left
    else if (rcvData.x_wp[rcvData.index]*30+20 < rcvData.xPos)
    {
        // need to turn
        if ((rcvData.orient < 44 || rcvData.orient > 46) && !rcvData.turning)
        {
            turn ((225 - rcvData.orient)%180);
        }
        // off center, and pointing moreso
        else if ((rcvData.yPos > rcvData.y_wp[rcvData.index]*30+20) && (rcvData.orient > 47) && !rcvData.turning)
        {
            turn(177);
        }
        // **optional code**
        else if ((rcvData.yPos < rcvData.y_wp[rcvData.index]*30+10) && (rcvData.orient < 43) && !rcvData.turning)
        {
            turn(3);
        }
        // already going left
        else
        {
            rcvData.turning = false;
            goForward(rcvData.xPos - rcvData.x_wp[rcvData.index]*30-15);
        }
    }
    // going right
    else if (rcvData.x_wp[rcvData.index]*30+10 > rcvData.xPos)
    {
        // need to turn
        if ((rcvData.orient < 134 || rcvData.orient > 136) && !rcvData.turning)
        {
            turn ((315 - rcvData.orient)%180);
        }
        // off center, and pointing moreso
        else if ((rcvData.yPos > rcvData.y_wp[rcvData.index]*30+20) && (rcvData.orient > 137) && !rcvData.turning)
        {
            turn(177);
        }
        // **optional code**
        else if ((rcvData.yPos < rcvData.y_wp[rcvData.index]*30+10) && (rcvData.orient < 133) && !rcvData.turning)
        {
            turn(3);
        }
        // already going right
        else 
        {
            rcvData.turning = false;
            goForward(rcvData.x_wp[rcvData.index]*30+15 - rcvData.xPos);
        }
    }
    else if (rcvData.y_wp[rcvData.index] == rcvData.square_y && rcvData.x_wp[rcvData.index] == rcvData.square_x)
        rcvData.index++;
}


bool checkObs()
{
    if (!rcvData.config_ready)
        return false;
    int i;
    for (i =0; i<rcvData.numObs; i++)
    {
        if (rcvData.obs_x[i] == 6 || rcvData.obs_y[i] == 6)
            return false;
    }
    return true;
}


void processCommand()
{
    int i;
    rcvData.type_in = rcvData.message[1];
    rcvData.ack_no = rcvData.message[2]<<8 + rcvData.message[3];
    rcvData.x_in = rcvData.message[4];
    rcvData.y_in = rcvData.message[5];
    rcvData.angle_in = rcvData.message[6];
    
    // Don't proceed if done
    if (rcvData.done)
    {
        stopAll();
        return;
    }
    
    if (rcvData.clock < rcvData.timer_threshold)
    {
        return;
    }
    
    // Lead Rover Position from Sensors
    if (rcvData.type_in == 0x10)
    {
        rcvData.xPos = rcvData.x_in;
        rcvData.yPos = rcvData.y_in;
        rcvData.orient = rcvData.angle_in;
        rcvData.loc_ready = true;
        // Determines current grid square
        rcvData.square_x = rcvData.xPos/30;
        rcvData.square_y = rcvData.yPos/30;
        rcvData.board [rcvData.square_x][rcvData.square_y] = '1';
        
        //  Calls course correction if needed;
        if (!rcvData.turning && rcvData.started && ((rcvData.orient > 3 && rcvData.orient < 42) || (rcvData.orient > 48 && rcvData.orient < 87) || (rcvData.orient > 93 && rcvData.orient < 132) || (rcvData.orient > 138 && rcvData.orient < 177)))
            stop();
        maintainLine();
        return;
    }
    
    // Initial configuration
    if (rcvData.type_in == 0x00)
    {
        rcvData.numObs = rcvData.x_in;
        rcvData.numToks = rcvData.y_in;
        rcvData.config_ready = true;
        return;
    }
    
    // Obstacle 1 placement
    if (rcvData.type_in == 0x11)
    {
        rcvData.obs_x[0] = rcvData.x_in;
        rcvData.obs_y[0] = rcvData.y_in;
        rcvData.obs_square_x[0] = rcvData.x_in/30;
        rcvData.obs_square_y[0] = rcvData.y_in/30;
        rcvData.obs_ready = checkObs();
        return;
    }
    
    // Obstacle 2 placement
    if (rcvData.type_in == 0x12)
    {
        rcvData.obs_x[1] = rcvData.x_in;
        rcvData.obs_y[1] = rcvData.y_in;
        rcvData.obs_square_x[1] = rcvData.x_in/30;
        rcvData.obs_square_y[1] = rcvData.y_in/30;
        rcvData.obs_ready = checkObs();
        return;
    }
    
    // Obstacle 3 placement
    if (rcvData.type_in == 0x13)
    {
        rcvData.obs_x[2] = rcvData.x_in;
        rcvData.obs_y[2] = rcvData.y_in;
        rcvData.obs_square_x[2] = rcvData.x_in/30;
        rcvData.obs_square_y[2] = rcvData.y_in/30;
        rcvData.obs_ready = checkObs();
        return;
    }
    
    // Lead Rover Distance Moved
    if (rcvData.type_in == 0x20)
    {
        if (rcvData.x_in == 0x00 && rcvData.y_in == 0x00 && rcvData.angle_in == 0x00)
        {
            rcvData.mov_ready = true;
        }
        else
        {
            rcvData.mov_ready = false;
            return;
        }
        if (rcvData.loc_ready && rcvData.obs_ready && rcvData.config_ready && !rcvData.started)
        {
            rcvData.started = true;
            // Builds way-points to navigate
            setPoints();
        }
        else if (rcvData.loc_ready && rcvData.mov_ready && rcvData.started)
        {
            // Navigates way-points
            goToGoal();
        }
        return;
    }
    
    // Lead Rover Found Token
    if (rcvData.type_in == 0x21)
    {
        for (i = 0; i < rcvData.numToks; i++)
        {
            // already on list
            if (rcvData.square_x == rcvData.tok_x[i] && rcvData.square_y == rcvData.tok_y[i])
                return;
        }
        // Add to list
        rcvData.tok_x[rcvData.token_index] = rcvData.square_x;
        rcvData.tok_y[rcvData.token_index] = rcvData.square_y;
        token();
        rcvData.token_index++;
        return;
    }
    
    // Follow Rover retrieved token
    if (rcvData.type_in == 0x40)
    {
        rcvData.retrieved_index++;
        // All tokens retrieved
        if (rcvData.retrieved_index == rcvData.numToks)
        {
            rcvData.done = true;
            stopAll();
        }
        rcvData.sequence_out++;
        addQSnd(START);
        addQSnd(0x77);
        addQSnd(rcvData.sequence_out/256);
        addQSnd(rcvData.sequence_out%256);
        addQSnd(rcvData.retrieved_index);
        addQSnd(rcvData.numToks);
        addQSnd(0x00);
        addQSnd(END);
        return;
    }
}

// Assembles instructions as char from uart arrive
void checkReceive(char letter)
{
    
    // Starts new received word when start character arrives
    if (letter == START)
    {
        rcvData.currentIndex = 0;
        rcvData.message[rcvData.currentIndex] = letter;
        rcvData.currentIndex++;
        return;
    }
    // More than 8 bytes without an end byte
    if (rcvData.currentIndex > 7)
    {
        rcvData.currentIndex = 0;
        return;
    }
    
    // Stores current letter in word
    rcvData.message[rcvData.currentIndex] = letter;
    
    // Indicates word has been properly assembled
    if (letter == END && rcvData.currentIndex == 7)
    {
        rcvData.currentIndex = 0;
        // Sends message out
        processCommand();
    }
    
    rcvData.currentIndex++;
    
}

// Event loop to read from message received queue
void RECEIVE_Tasks ( void )
{
    char letter;
    while(1)
    {
        xQueueReceive(rcvData.rcvQueue, &letter, portMAX_DELAY);
        checkReceive(letter);
    }
}
 

