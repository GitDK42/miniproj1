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
    int id;
    double t_serv;
    double t_arrive;
    double t_depart;
    // constructor for creating packets
    Packet (int, double, double, double);
    // overload < and > operators to use packets in priority_queue
    bool operator< (const Packet& other) const {
        return this->t_serv < other.t_serv;
    }
    bool operator> (const Packet& other) const {
        return this->t_serv > other.t_serv;
    }
};

Packet::Packet(int id, double t_serv, double t_arrive, double t_depart) {
    this->id = id;
    this->t_serv = t_serv;
    this->t_arrive = t_arrive;
    this->t_depart = t_depart;
}

/* Fucntion for the actual simulation. Upon arrival packets are routed to Router 1 or Router 2. If a packet is already being serviced at that router, placed in queue. 
 * numpkts: Number of packets sent in simulation
 * lambda: Poisson Process parameter for arrival time
 * mu: Exponentiail Process parameter for departure/service time
 * phi: Fixed probability packets are routed to first router upon arrival
 */
void simulation(int numpkts, int lambda, int mu, int phi) {
    // TODO: this
/*    std::cout << "number of packets = " << numpkts << std::endl;
    std::cout << "lambda = " << lambda << std::endl;
    std::cout << "mu = " << mu << std::endl;
    std::cout << "phi = " << phi << std::endl; */
    std::random_device rd;
    std::uniform_int_distribution<int> u_dist(0, 100);
    std::exponential_distribution<double> p_dist(lambda);
    std::exponential_distribution<double> e_dist(mu);

    int link = u_dist(rd);
    double arrival = p_dist(rd);
    double departure = e_dist(rd);

    std:: cout << u_dist(rd) << ", " << phi << std::endl;
    std:: cout << p_dist(rd) << ", " << arrival << std::endl;
    std:: cout << e_dist(rd) << ", " << departure << std::endl;
}

int main() {
    int numpkts, lambda, mu, phi;
   /* std::cout << "Enter number of packets in simulation: ";
    std::cin >> numpkts;
    std::cout << "Enter lambda value: ";
    std::cin >> lambda;
    std::cout << "Enter mu value: ";
    std::cin >> mu;
    std::cout << "Enter phi value: ";
    std::cin >> phi;
    //simulation(numpkts, lambda, mu, phi);
    */
    // testing priority queue stuff
 //   std::priority_queue<Packet> test(std::greater<Packet>());
    std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> test; 
    Packet p1(5, 2.3, 4.1, 5.7);
    Packet p2(6, 2, 1.3, 6.2);
    
    std::cout << test.size() << std::endl;
    test.push(p1);
    std::cout << test.size() << std::endl;
    test.push(p2);
    std::cout << test.size() << std::endl;
    
    std::cout << test.empty() << std::endl;
    std::cout << test.top().id << std::endl;
    test.pop();
    std::cout << test.top().id << std::endl;
    std::cout << test.size() << std::endl;
    test.pop();
    std::cout << test.empty() << std::endl;
    
    return 0;
}

