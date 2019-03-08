The following information will be available:

* Router ID, which is its index into the tables below, is given at the command line.
* Number of nodes, N, in the topology will be given by the command line.
* Table with costs, NxN, will be obtained from file1 (name given at the commandline).
* Table with machines, names, IP addresses, and port numbers, Nx3, will beobtained from file2 (name given at the command line).

Your main data will be:

* Neighbor cost table –  contains the cost from every node to every node, initially obtained from file1.
* Least cost array –  obtained with the link state algorithm.

Your code will have 3 threads:

* Thread 1 loops forever. It receives messages from other nodes and updates the neighbor cost table. When receiving a new cost c from x to neighbor y, it should update the cost in both costs: x to y and y to x.
* Thread 2 reads a new change from the keyboard every 10 seconds, updates the neighbor cost table, and sends messages to the other nodes using UDP. It finishes 30 seconds after executing 2 changes. You may execute this part in the main thread.
* Thread 3 loops forever. It sleeps for a random number of seconds (10-20), run the algorithm to update the least costs. After the algorithm executes it outputs the current least costs.

# Thread 1

* while 1
  * recvfrom(...)
  * lock table
  * update neighbor cost c from (x to y)
    * cost[x][y] = c
    * cost[y][x] = c
  * unlock table

# Thread 2 (main)

* while 1
  * getline() following format `<neighbor><space><new cost><new line>`
  * lock table
  * update neighbor cost c from (x to y) as above
  * sendto() `<Routers’ ID><neighbor ID><new cost>` other nodes
  * unlock table

# Thread 3

* while 1
  * sleep rand(10, 20)
  * lock table
  * update least costs
  * unlock table
  * output current least costs

cost table

<i, j> = cost between i and j

<0,0> <0,1> <0,2>
<1,0> <1,1> <1,2>
<2,0> <2,1> <2,2>
