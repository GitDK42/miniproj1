/* ECE 561 Mini-Project 1
 * Simulation of a simple networking system with two routers 
 * 2015/10/08
 * Daniel Kellett
 */

#include <iostream>
#include <queue>
#include <functional>
#include <random>
#include <vector>

/* Struct for the packets being sent through the system.
 * t_arrive: time of arrival of packet
 * t_serv: Exp. generated random variable represeting time spent processinng packet/size of packet.
 * t_depart: time of departure of packet
 * t_event: time this packet's next event will occur (arrive/depart)
 * link: boolean value to indicate if packet is routed to link 1 (true) or link 2 (false)
 * arriving: boolean value to indicate if packet is arriving/departing
 */
struct Packet {
    double t_arrive;
    double t_serv;
    double t_depart;
    double t_event;
    bool link, arriving;
    // constructor for creating packets
    Packet (bool, double, double);
    // overload < and > operators to use packets in priority_queue
    bool operator< (const Packet& other) const {
        return this->t_event < other.t_event;
    }
    bool operator> (const Packet& other) const {
        return this->t_event > other.t_event;
    }
};

Packet::Packet(bool link, double t_arrive, double t_serv) {
    this->t_arrive = t_arrive;
    this->t_serv = t_serv;
    this->t_event = this->t_arrive;
    this->t_depart = -1;
    this->link = link;   // true = link 1
    this->arriving = true;
}
double approxRollingAvg (double avg, double new_sample) {
    
    avg -= avg / 10;
    avg += new_sample / 10;

    return avg;
}
/* Function to decide which router new packet is sent to
 *
 */
bool pickLink(int phi) {
    std::random_device rd;
    std::uniform_int_distribution<int> u_dist(0,100);
    if (u_dist(rd) > phi) {
        return false;
    }
    return true;
}

/* Fucntion to get arrival time for new packet
 *
 */
double getArrivalTime(int lambda) {
    double t_arrive;
    std::random_device rd;
    std::exponential_distribution<double> p_dist(lambda);
    t_arrive = p_dist(rd);

    return t_arrive;
}

/* Fucntion to get departure time for packet
 *
 */
double getServTime(int mu) {
    double t_serv;
    std::random_device rd;
    std::exponential_distribution<double> e_dist(mu);
    t_serv = e_dist(rd);

    return t_serv;
}

/* Fucntion for the actual simulation. Upon arrival packets are routed to Router 1 or Router 2. If a packet is already being serviced at that router, placed in queue. 
 * numpkts: Number of packets sent in simulation
 * lambda: Poisson Process parameter for arrival time
 * mu: Exponentiail Process parameter for departure/service time
 * phi: Fixed probability packets are routed to first router upon arrival
 */
void simulation() {
    // define system parameters
    int numpkts=10500, lambda=8, mu=5, phi=50, size=20;
/*    std::cout << "Enter number of packets in simulation: ";
    std::cin >> numpkts;
    std::cout << "Enter lambda value: ";
    std::cin >> lambda;
    std::cout << "Enter mu value: ";
    std::cin >> mu;
    std::cout << "Enter phi value: ";
    std::cin >> phi;
    std::cout << "Enter buffer size: ";
    std::cin >> size;
*/
    // create event list and buffers
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> eventList;
    std::queue<Packet> b1, b2;
    
    // define system information variables
    double t_sys = 0;            // system time
    double delay1=0, delay2=0;   // delay variables for buffers
    int link1=0, link2=0;        // number of packets passing through.
    int numBlock1=0, numBlock2=0;// number of packets blocked 
    int pktSent = 0;             // number of packets sent
    double avgDelay1=0, avgDelay2=0;
    double avgPkts1=0, avgPkts2=0; 
    // begin simulation
    // ignore first X pkts (X >= 500 for 5500 sent)
    int x = 500;
//    std::cout << "before ignore while" << std::endl;
    int loopNum = 0;
    double avg_p=0;
    double avg_e=0;
    while(pktSent <= x-1) {
//        std::cout << "loop number: " << loopNum << std::endl;
        loopNum++;
        if (pktSent == 0) {
            Packet p(pickLink(phi), getArrivalTime(lambda)+t_sys, getServTime(mu));
            eventList.push(p);
            pktSent++;             // increment number of packets sent
        }
 
        Packet temp = eventList.top();
        // determine if next event is arriving or departing packet
        if (temp.arriving) {
            // determine link to send arriving packet to.
//            std::cout << "packet incoming!" << std::endl;
            if(temp.link) {
//                std::cout << "arrival at link 1!" << std::endl;
                // determine if buffer is full
                if (b1.size() < size+1) {
//                    std::cout << "space available!" << std::endl;
                    delay1 += temp.t_serv; // update buffer delay
                    t_sys = temp.t_arrive; // update system time
                    // calculate departure time of pkt.
                    temp.t_depart = delay1 + t_sys;
                    // change from arriving to departing
                    temp.arriving = false;
                    // update event list
                    temp.t_event = temp.t_depart;
                    eventList.pop();
                    eventList.push(temp);
                    // add packet to buffer
                    b1.push(temp);   // push pkt to buffer
                } else {
//                    std::cout << "we're blocked!" << std::endl;
                    eventList.pop(); // update event list.
                }
            } // determine if buffer is full
            else if (b2.size() < size+1) {
//                 std::cout << "arrival at link 2!" << std::endl;
                    delay2 += temp.t_serv; // update buffer delay
                    t_sys = temp.t_arrive; // update system time
                    // calculate departure time of pkt.
                    temp.t_depart = delay2 + t_sys;
                    // change from arriving to departing
                    temp.arriving = false;
                    // update event list
                    temp.t_event = temp.t_depart;
                    eventList.pop();
                    eventList.push(temp);
                    // add packet to buffer
                    b2.push(temp);   // push pkt to buffer
                } else {
                    eventList.pop(); // update event list.
                }
            // new packet enters event list
            Packet p(pickLink(phi), getArrivalTime(lambda)+t_sys, getServTime(mu));
            eventList.push(p);
            pktSent++;             // increment number of packets sent
        }
        // else packet is departing
        else {
            // determine link departing from.
            if(temp.link) {
                delay1 -= temp.t_serv;       // update buffer delay
                t_sys = temp.t_depart;       // update system time
                b1.pop();                    // update buffer
                eventList.pop();             // update event list
            } else {
                delay2 -= temp.t_serv;       // update buffer delay
                t_sys = temp.t_depart;       // update system time
                b2.pop();                    // update buffer
                eventList.pop();             // update event list
            }
        }
    } // end of while loop

// ----------------------- BEGIN TRACKED ---------------------------//
    avgDelay1 = delay1;
    avgDelay2 = delay2;
    avgPkts1 = b1.size();
    avgPkts2 = b2.size();
    int loopCount = 0;

    // tracked simulation
    while(!eventList.empty()) {
        loopCount++;
        Packet temp = eventList.top();
       // determine if next event is arriving or departing packet
       if (temp.arriving) {
           // determine link to send arriving packet to.
           if(temp.link) {
               // determine if buffer is full
               if (b1.size() < size+1) {
                   delay1 += temp.t_serv; // update buffer delay
                   t_sys = temp.t_arrive; // update system time
                   // calculate departure time of pkt.
                   temp.t_depart = delay1 + t_sys;
                   // change from arriving to departing
                   temp.arriving = false;
                   // update event list
                   temp.t_event = temp.t_depart;
                   eventList.pop();
                   eventList.push(temp);
                   // add packet to buffer
                   b1.push(temp); // push pkt to buffer
               } else {
                   numBlock1++;     // incr. pkts blocked by b1
                   eventList.pop(); // update event list.
               }
           } // determine if buffer is full
           else if (b2.size() < size+1) {
                   delay2 += temp.t_serv; // update buffer delay
                   t_sys = temp.t_arrive; // update system time
                   // calculate departure time of pkt.
                   temp.t_depart = delay2 + t_sys;
                   // change from arriving to departing
                   temp.arriving = false;
                   // update event list
                   temp.t_event = temp.t_depart;
                   eventList.pop();
                   eventList.push(temp);
                   // add packet to buffer
                   b2.push(temp); // push pkt to buffer
               } else {
                   numBlock2++;     // incr. pkts blocked by b2
                   eventList.pop(); // update event list.
               }
            if(pktSent < numpkts) {   
                // new packet enters event list
                Packet p(pickLink(phi), getArrivalTime(lambda)+t_sys, getServTime(mu));
                eventList.push(p);
                pktSent++;            // increment number of packets sent
            }
       }
       // else packet is departing
       else {
           // determine link departing from.
           if(temp.link) {
               delay1 -= temp.t_serv;       // update buffer delay
               t_sys = temp.t_depart;       // update system time
               b1.pop();                    // update buffer
               eventList.pop();             // update event list
               link1++;                     // incr. # pkts passed
           } else {
               delay2 -= temp.t_serv;       // update buffer delay
               t_sys = temp.t_depart;       // update system time
               b2.pop();                    // update buffer
               eventList.pop();             // update event list
               link2++;                     // incr. # pkts passed
           }
       }
       if (loopCount%10 == 0) {
           avgPkts1 = approxRollingAvg(avgPkts1, b1.size());
           avgPkts2 = approxRollingAvg(avgPkts2, b2.size());
           avgDelay1 = approxRollingAvg(avgDelay1, delay1);
           avgDelay2 = approxRollingAvg(avgDelay2, delay2);
       }

    } // end of while loop

    int totalBlocked = numBlock1 + numBlock2;
    int total_1 = link1 + numBlock1;        // total sent to link 1
    int total_2 = link2 + numBlock2;        // total sent to link 2
    int total_sys = link1 + link2;          // total sent through sys
    int total_sent = totalBlocked + total_sys;

    double sys_pb = (double)totalBlocked/total_sent; // Pb for system
    double link1_pb = (double)numBlock1/total_1;      // Pb for link 2
    double link2_pb = (double)numBlock2/total_2;      // Pb for link 1
    double avgSysDelay = (avgDelay1+avgDelay2)/2;

    double throughput1 = link1/t_sys;
    double throughput2 = link2/t_sys;
    double throughputSys = throughput1+throughput2;
    printf("|============================================|\n");
    printf("|=============SIMULATION RESULTS=============|\n");
    printf("|============================================|\n\n");
    printf("System Parameters:\n");
    printf(" Arrival rate lambda: %i\n", lambda);
    printf(" Departure rate mu:   %i\n", mu);
    printf(" Buffer sizes (both): %i\n\n", size);
    printf(" Total Sent    :  %i\n", numpkts);
    printf(" Packets Ignored: %i\n", x);
    printf(" Total duration:  %f seconds\n\n", t_sys);

    printf("Link 1 Statistics (phi = %i\%): \n", phi);
    printf(" Total Blocked:        %i\n", numBlock1);
    printf(" Total Arrivals:       %i\n", total_1);
    printf(" Blocking Probability: %f\% \n", link1_pb*100);
    printf(" Average Delay Time:   %f seconds\n", avgDelay1);
    printf(" Throughput:           %f pkts/s\n", throughput1); 

    printf("\nLink 2 Statistics (phi = %i\%): \n", 100-phi);
    printf(" Total Blocked:        %i\n", numBlock2);
    printf(" Total Arrivals:       %i\n", total_2);
    printf(" Blocking Probability: %f\% \n", link2_pb*100);
    printf(" Average Delay Time:   %f seconds\n", avgDelay2);
    printf(" Throughput:           %f pkts/s\n", throughput2); 

    printf("\nSystem Statistics: \n");
    printf(" Total Blocked:        %i\n", totalBlocked);
    printf(" Total Arrivals:       %i\n", total_sent);
    printf(" Blocking Probability: %f\% \n", sys_pb*100);
    printf(" Average Delay Time:   %f seconds\n", avgSysDelay);
    printf(" Throughput:           %f pkts/s\n", throughputSys); 

}

int main() {
    simulation();

/*    // testing priority queue stuff
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> test; 
    Packet p1(5, 2.3, 4.1, 5.7);
    Packet p2(6, 2, 1.3, 6.2);
    
    std::cout << test.size() << std::endl;
    test.push(p1);
    std::cout << test.size() << std::endl;
    test.push(p2);
    std::cout << test.size() << std::endl;
    
    std::cout << test.empty() << std::endl;
    std::cout << test.top().t_serv << std::endl;
    test.pop();
    std::cout << test.top().t_serv << std::endl;
    std::cout << test.size() << std::endl;
    test.pop();
    std::cout << test.empty() << std::endl;
    std::cout << (p1 > p2) << std::endl;
*/
    return 0;
}

