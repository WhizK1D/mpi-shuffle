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
    5. Send and receive the no of items for each processor to be received
    6. Prepare the displacement vector for storing the offsets when receiving
    7. MPI_alltoallv() with send and receive buffer details
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
    std::vector<int> recCountVec(size, 0);

    int totalRecCount = 0;
    int totalSendCount = 0;


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
    for(int i = 0; i < size; i++)
    {
        std::cout << " Proc [" << rank << "] has " << countVec[i] << " elements for Proc [" << i << "]\n";
    }
    MPI_Barrier(Comm); // DEBUG: Step 2


    // 3. Sort inout based on hash % p so that displacement can be calculated
    // std::sort(inout.begin(), inout.end(), std::bind(hash_compare, std::placeholders::_1, std::placeholders::_2, hash, size));
    // Implement a lambda since compare function cannot have more than 2 arguments
    std::sort(inout.begin(), inout.end(), [size, hash](T &x, T &y){return ((hash(x) % size)) < ((hash(y) % size));});

    std::cout << "First element: " << inout[0] << " Last element: " << inout[inout.size() - 1] << " on processor " << rank << "\n";
    MPI_Barrier(Comm); // DEBUG: Step 3

    // 4. Calculate displacement in inout for each other processor to send

    for(int i=0, mod=0; i < inout.size(); i++)
    {
        if (( hash(inout[i]) % size) == mod)
        {
            continue;
        }
        else
        {
            mod += 1;
            displacementVec[mod] = i;
        }
    }
    for(int i = 0; i < displacementVec.size(); i++)
    {
        std::cout << "Proc [" << rank << "] has elements at " << displacementVec[i] << " for Proc [" << i << "]\n";
    }
    MPI_Barrier(Comm); // DEBUG: Step 4
    //TODO: Merge for-loops in Step 2 and Step 4 into one


    // 5. Send and receive the no of items for each processor to be received
    MPI_Status status;
    for(int i = 0; i <size; i++)
    {
        MPI_Sendrecv(&countVec[i], 1, MPI_INT, i, i,
                &recCountVec[i], 1, MPI_INT, i, rank, Comm, &status);
    }

    for(int i=0; i<size; i++)
    {
        std::cout << "Proc [" << rank << "] received " << recCountVec[i] << " from Proc [" << i << "]\n";
    }
    MPI_Barrier(Comm); // DEBUG: Step 5

    // Extra: calculate total items to be received and sent
    for (int i = 0; i<recCountVec.size(); i++)
    {
        totalRecCount += recCountVec[i];
        totalSendCount += countVec[i];
    }

    std::cout << "Proc [" << rank << "] will send total " << totalSendCount << " elements\n";
    std::cout << "Proc [" << rank << "] will receive total " << totalRecCount << " elements\n";

    // 6. Prepare the displacement vector for storing the offsets when receiving
    std::vector<int> recDisplacement(size, 0);

    for (int i = 0, sum = 0; i < size; i++)
    {
        recDisplacement[i] = sum;
        sum += recCountVec[i]; // Sum of items to receive till that point is the displacement
    }

    for (int i = 0; i<size; i++)
    {
        std::cout << "Displacement on [" << rank << "] for receiving from " << i << " is " << recDisplacement[i] << "\n";
    }
    MPI_Barrier(Comm);

    // TODO: Check if new buffer is needed for alltoallv and perform alltoallv()

    std::vector<T> recVector (totalRecCount, 0);

    int *recCountArr = &recCountVec[0];
    int *sendCountArr = &countVec[0];
    int *sendDispArr = &displacementVec[0];
    int *recDispArr = &recDisplacement[0];

    // TODO: Resolve bug for segfault
    MPI_Alltoallv(&inout, sendCountArr, sendDispArr, MPI_INT,
        &recVector, recCountArr, recDispArr, MPI_INT, Comm);

    std::cout << "Processor [" << rank << "] has received first element as " << recVector[0];
} // mpi_shuffle

#endif // A1_HPP
