/* ECE 561 Mini-Project 1
 * Simulation of a simple networking system with two routers 
 * 2015/10/08
 * Daniel Kellett
 */

#include <iostream>
#include <queue>
#include <functional>
#include <random>

/* Struct for the packets being sent through the system.
 * num: Integer representing the packet number (e.g. 500th packet)
 * t_serv: Exp. generated random variable represeting time spent processinng packet/size of packet.
 */
struct Packet {
    int id;
    double t_serv;
    double t_arrive;
    double t_depart;
};

/* Fucntion for the actual simulation. Upon arrival packets are routed to Router 1 or Router 2. If a packet is already being serviced at that router, placed in queue. 
 * numpkts: Number of packets sent in simulation
 * lambda: Poisson Process parameter for arrival time
 * mu: Exponentiail Process parameter for departure/service time
 * phi: Fixed probability packets are routed to first router upon arrival
 */
void simulation(int numpkts, int lambda, int mu, double phi) {
    // TODO: this
/*    std::cout << "number of packets = " << numpkts << std::endl;
    std::cout << "lambda = " << lambda << std::endl;
    std::cout << "mu = " << mu << std::endl;
    std::cout << "phi = " << phi << std::endl; */
    std::random_device rd;
    std::uniform_real_distribution<> uni_dis(0, 1);
    std::poisson_distribution<> poisson(lambda);
    std::exponential_distribution<> exponential(mu);

    std:: cout << uni_dis(gen) << std::endl;
    std:: cout << poisson(gen) << std::endl;
    std:: cout << exponential(gen) << std::endl;
}

int main() {
    int numpkts, lambda, mu;
    double phi;
    std::cout << "Enter number of packets in simulation:";
    std::cin >> numpkts;
    std::cout << "Enter lambda value:";
    std::cin >> lambda;
    std::cout << "Enter mu value:";
    std::cin >> mu;
    std::cout << "Enter phi value:";
    std::cin >> phi;
    simulation(numpkts, lambda, mu, phi);
   // std::priority_queue<Packet> test;

   return 0;
}

