/*Task Description*/

/*A non-empty array A consisting of N integers is given. 
The consecutive elements of array A represent consecutive cars on a road.
Array A contains only 0s and/or 1s:
0 represents a car traveling east,
1 represents a car traveling west.
The goal is to count passing cars. 
We say that a pair of cars (P, Q), where 0 ≤ P < Q < N, is passing when P is traveling to the east and Q is traveling to the west.
For example, consider array A such that:
  A[0] = 0
  A[1] = 1
  A[2] = 0
  A[3] = 1
  A[4] = 1
We have five pairs of passing cars: (0, 1), (0, 3), (0, 4), (2, 3), (2, 4).
Write a function:
int solution(vector<int> &A);
that, given a non-empty array A of N integers, returns the number of pairs of passing cars.
The function should return −1 if the number of pairs of passing cars exceeds 1,000,000,000.*/

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

int solution(vector<int> &A)
{
    int lenA = A.size();
    if (lenA == 1)
    {
        return 0;
    }
    int count = 0;
    int sum_1 = accumulate(A.begin(), A.end(), 0);
    for (int i = 0; i < lenA; i++)
    {
        if (A[i] == 1)
        {
            sum_1 -= 1;
        }
        else if (A[i] == 0)
        {
            count += sum_1;
        }
    }
    if (count > (1E9))
    {
        return -1;
    }
    return count;
}

int main(int argc, char const *argv[])
{
    vector<int> A{0, 1, 0, 1, 1};
    cout << solution(A);
    cout << endl;
    return 0;
}
