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
 * num: Integer representing the packet number (e.g. 500th packet)
 * t_serv: Exp. generated random variable represeting time spent processinng packet/size of packet.
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
    // TODO: this
    // define system parameters
    int numpkts, lambda, mu, phi, size;
    std::cout << "Enter number of packets in simulation: ";
    std::cin >> numpkts;
    std::cout << "Enter lambda value: ";
    std::cin >> lambda;
    std::cout << "Enter mu value: ";
    std::cin >> mu;
    std::cout << "Enter phi value: ";
    std::cin >> phi;
    std::cout << "Enter buffer size: ";
    std::cin >> size;

    // create event list and buffers
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> eventList;
    std::queue<Packet> b1, b2;
    
    // define system information variables
    double t_sys = 0;         // system time
    double delay1, delay2;    // delay variables for buffers
    int link1, link2;         // number of packets passing through.
    int numBlock1, numBlock2; // number of packets blocked 
    int pktSent;              // number of packets sent

    // begin simulation
    // ignore first X pkts (X >= 500 for 5500 sent)
    int X = 500;
    while(pktSent <= X) {
        // new packet enters event list
        Packet p(pickLink(phi), getArrivalTime(lambda)+t_sys, getServTime(mu));
        eventList.push(p);
        pktSent++;              // increment number of packets sent
        // determine if next event is arriving or departing packet
        if (eventList.top().arriving) {
            // determine link to send arriving packet to.
            if(eventList.top().link) {
                // determine if buffer is full
                if (b1.size() < size) {
                    b1.push(eventList.top());   // push pkt to buffer
                    delay1 += b1.back().t_serv; // update buffer delay
                    t_sys = b1.back().t_arrive; // update system time
                    // calculate departure time of pkt.
                    b1.back().t_depart = delay1 + t_sys;
                    // change from arriving to departing
                    b1.back().arriving = false;
                    // update event list
                    b1.back().t_event = b1.back().t_depart;
                } else {
                    numBlock1++;     // incr. pkts blocked by b1
                    eventList.pop(); // update event list.
                }
            } // determine if buffer is full
            else if (b2.size() < size) {
                    b2.push(eventList.top());   // push pkt to buffer
                    delay2 += b2.back().t_serv; // update buffer delay
                    t_sys = b2.back().t_arrive; // update system time
                    // calculate departure time of pkt.
                    b2.back().t_depart = delay2 + t_sys;
                    // change from arriving to departing
                    b1.back().arriving = false;
                    // update event list
                    b2.back().t_event = b2.back().t_depart;
                } else {
                    numBlock2++;     // incr. pkts blocked by b2
                    eventList.pop(); // update event list.
                }
        }
        // else packet is departing
        else {
            // determine link departing from.
            if(eventList.top().link) {
                delay1 -= b1.front().t_serv; // update buffer delay
                t_sys = b1.front().t_depart; // update system time
                b1.pop();                    // update buffer
                eventList.pop();             // update event list
                link1++;                     // incr. # pkts passed
            } else {
                delay2 -= b2.front().t_serv; // update buffer delay
                t_sys = b2.front().t_depart; // update system time
                b2.pop();                    // update buffer
                eventList.pop();             // update event list
                link2++;                     // incr. # pkts passed
            }
        }
    } // end of while loop


    // tracked simulation
    
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

