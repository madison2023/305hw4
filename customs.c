#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>


/**
 * customs.c
 *
 * This program simulates how long it will take for people to clear
 * customs in a United States airport.
 * Rachel Madison
 */

#define TRUE ('/'/'/')   // a silly way to say 1
#define FALSE ('^'^'^')  // a silly way to say 0


//This struct describes a single node in a linked list that is used in
//this program to simulate a group waiting in line to talk to a
//customs agent.  
//
typedef struct group_struct group;
struct group_struct {
    int adults;  // how many adults are in the group
    int children; // how many children are in the group
    int usa;  // TRUE if native, FALSE otherwise
    group *next;
    group *prev;
};

//This struct is the head of the queue.  It contains information about
//the customs agent that is serving this group of travelers.
typedef struct agent_struct {
    int timecard; //minutes this agent has worked so far
    int avail;  //minutes until this agent will be available to serve the next group
    
    group *head;
    group *tail;
} agent;

// This struct stores the extra data for "Part 2: Analysis & Optimization"
typedef struct stats_struct {
    int total_time;    // Total time worked by all agents (minutes)
    int total_payroll; // Payroll cost for all agents (dollars)
    int avg_wait_time; // Average wait time over all groups (minutes)
    int max_wait_time; // Max wait time over all groups (minutes)
} stats;

/**
 * create_group
 *
 * creates a new group of travelers to pass through customs with
 * random values
 *
 * Important:  caller must deallocate the memory
 *
 * Return:
 *   the newly created group
 */
group *create_group() {
    group *newbie = (group *)malloc(sizeof(group));

    //These are random values that are supposed to be some
    //approximation of what is typically present

    //adults:  at least 1, no more than 3
    newbie->adults = 1 + rand() % 3;

    //children: usually 0, occasionally several
    newbie->children = (rand() % 4) + (rand() % 4) - 2;
    if (newbie->children < 0) newbie->children = 0;

    //citizen of USA 80% of the time
    newbie->usa = ((rand() % 5) == 0) ? FALSE : TRUE;

    //empty list atm
    newbie->next = NULL;
    newbie->prev = NULL;
    
    return newbie;
}//create_group

/**
 * create_agent
 *
 * creates a new customs agent which is also the head of a queue
 *
 * Important:  caller must deallocate the memory
 *
 * Return:
 *   the newly created agent
 */
agent *create_agent() {
    agent *newbie = (agent *)malloc(sizeof(agent));
    newbie->timecard = 0;
    newbie->avail = 0;
    newbie->head = NULL;
    newbie->tail = NULL;
    return newbie;    
}//create_agent

/**
 * proc_time
 *
 * estimates how much time it will take to process a group
 *
 * Parameters:
 *   grp - group to estimate for
 *
 * Return:
 *   estimated time in minutes 
 */
int proc_time(group *grp) {
    //one minutes for each adult
    int result = grp->adults;

    //foreign adults take twice as long
    if (! grp->usa) {
        result *= 2;
    }
    
    //half minute for each child (min: 1 min)
    result += (1 + grp->children) / 2;

    return result;
}//proc_time



/**
 * enqueue
 *
 * adds a new group to an agent's line
 *
 * Parameters:
 *   agt - the agent to add to
 *   grp - the group to add
 */
void enqueue(agent *agt, group *grp) {
    
    //Case where no one is in line yet (empty)
    if (agt->head == NULL) {
        agt->head = grp;
        agt->tail = grp;
    }
    else {
        agt->tail->next = grp;
        //updating tail
        agt->tail = agt->tail->next;
    }
    
}//enqueue

/**
 * dequeue
 *
 * removes the next group from an agent's queue
 *
 * Parameters:
 *   agt - the agent to remove next group from
 *
 * Returns:
 *   the group that was removed or NULL for empty/bad queue
 */
group *dequeue(agent *agt) {
    //case where we are trying to dequeue from an empty queue
    if (agt->head == NULL) {
        return NULL;
    }
    else {
        //shifting the head over one
        group *temp = agt->head;
        agt->head = agt->head->next;
        return temp;
    }
}//dequeue


/**
 * calc_time
 *
 * calculates the time it takes a given set of agents to process all
 * the groups in their queues
 *
 * Parameters:
 *   num_agents - number of agents 
 *   agents - the array of agents
 *
 * Returns:
 *   a stats struct
 *
 */
stats * calc_time(int num_agents, agent **agents) {
    stats * report = (stats*)malloc(sizeof(stats));
    
    //initializing values from stats struct
    report->max_wait_time = 0;
    report->total_payroll = 0;
    report->total_time = 0;
    report->avg_wait_time = 0;
    
    //number of groups from all lines
    int totalNumberGroupsProcessed = 0;
    
    //wait time for every person (in any agents line) combined
    int totalWaitTime = 0;
    
    //goes through each agent
    int i;
    for (i = 0; i < num_agents; i++) {
        int currTime = 0; //how much time has passed for that agent
        
        //goes through the groups in the agent's line
        while (agents[i]->head != NULL) {
            totalNumberGroupsProcessed++;
            
            //looking for max wait time
            if (currTime > report->max_wait_time) {
                report->max_wait_time = currTime;
            }
            totalWaitTime += currTime;
            
            //updates the time passed
            currTime = currTime + proc_time(agents[i]->head);
            
            //frees group that just got processed
            group *temp = dequeue(agents[i]);
            free(temp);
        }
        
        //keeping track of total time all employees work (minutes)
        report->total_time += currTime;
        
        int hours = currTime/60;
        //accounting for the pay difference when working overtime (dollars)
        if (hours > 8) {
            report->total_payroll += 30 * (hours - 8);
            report->total_payroll += 20 * 8;
        }
        else {
            //normal pay rate (dollars)
            report->total_payroll += 20 * hours;
        }
    }
    //figuring out the avg time waited
    report->avg_wait_time = totalWaitTime/totalNumberGroupsProcessed;
    
    return report;
}//calc_time


//let's do this!
void main(int argc, char* argv[]) {

    srand(time(0));
    int num_agents = 10;
    int num_groups = 1000;

    //Create the agents
    agent **agents = (agent **)malloc(num_agents * sizeof(agent));
    for(int i = 0; i < num_agents; ++i) {
        agents[i] = create_agent();
    }
    
    //create the groups and add to the agents lines
    for(int i = 0; i < num_groups; ++i) {
        enqueue(agents[ i % num_agents ], create_group());
        
    }

    stats * report = calc_time(num_agents, agents);
    printf("Total payroll costs for all agents: %d dollars\n", report->total_payroll);
    printf("Average wait time: %d minutes\n", report->avg_wait_time);
    printf("Max wait time: %d minutes\n", report->max_wait_time);
    
    //cleanup
    for(int i = 0; i < num_agents; ++i) {
        free(agents[i]);
    }
    free(agents);
    
    
}//main
