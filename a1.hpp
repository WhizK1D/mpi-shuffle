/*  Abhyudaya
 *  Mourya
 *  amourya
 */

#ifndef A1_HPP
#define A1_HPP

#include <vector>
#include <iostream>
#include <algorithm>

template <typename T, typename Hash>
bool hash_compare(T &a, T &b, Hash hash, int size)
{
    return ((hash(a) % size) < (hash(b) % size));
}


template <typename T, typename Hash>
void mpi_shuffle(std::vector<T>& inout, Hash hash, MPI_Datatype Type, MPI_Comm Comm) {

    /*Basic algorithm to shuffle data:
    1. Print length of each processor's inout
    2. Calculate no of elements each processor has for each other processor
    3. Sort inout based on hash % p so that displacement can be calculated
    4. Calculate displacement in inout for each other processor to send
    5. Ready the receive buffer same as send buffer above
    6. MPI_alltoallv() with send and receive buffer details
    */

    /*
     * size = no of processors
     * rank = index of processor in execution
     * n = size of the data
     * */
    int size=0, rank=0;
    MPI_Comm_size(Comm, &size);
    MPI_Comm_rank(Comm, &rank);

    std::vector<int> countVec(size, 0);
    std::vector<int> displacementVec(size, 0);

/*
    --- Possibly useless code ---
    n = inout.size();
    int rem = n % size;
    int quotient = n/size;

    if(rank < rem)
    {
        start = rank * (quotient + 1);
        stop = start + quotient;
    }
    else
    {
        start = rank * quotient + rem;
        stop = start + (quotient -1);
    }
*/

    // 1. Print length of each processor's inout
    std::cout << "[" << rank << "] Length of inout: " << inout.size() << "\n";

    MPI_Barrier(Comm); // DEBUG: Step 1

    // 2. Calculate no of elements each processor has for each other processor
    for(int i = 0; i < inout.size(); i++)
    {
        int temp = 0;
        temp = (hash(inout[i]) % size);
        countVec[temp] += 1;
    }

    // Below for-loop prints the number of items each processor has for other processors
    for(int i = 0; i < countVec.size(); i++)
    {
        std::cout << " Proc [" << rank << "] has " << countVec[i] << " elements for Proc [" << i << "]\n";
    }

    MPI_Barrier(Comm); // DEBUG: Step 2

    // 3. Sort inout based on hash % p so that displacement can be calculated
    // std::sort(inout.begin(), inout.end(), std::bind(hash_compare, std::placeholders::_1, std::placeholders::_2, hash, size));
    // Implement a lambda since compare function cannot have more than 2 arguments
    std::sort(inout.begin(), inout.end(), [size, hash](T &x, T &y){return ((hash(x) % size)) < ((hash(y) % size));});

    std::cout << "First element: " << inout[0] << " Last element: " << inout[inout.size() - 1] << " on processor " << rank << "\n";

    for(int i = 0; i < countVec.size(); i++)
    {
        std::cout << " Proc [" << rank << "] has " << displacementVec[i] << " elements for Proc [" << i << "]\n";
    }
    MPI_Barrier(Comm); // DEBUG: Step 3
    // for(int i = 0; i < n; i++)
    // {
    //     if(((rank + i) % 2) == 0)
    //     {
    //         if(rank < n-2)
    //         {
    //             MPI_Sendrecv_replace(inout, 1, MPI_INT, rank, rank, rank + 1, rank + 1, Comm);
    //         }
    //     }
    // }
} // mpi_shuffle

#endif // A1_HPP
