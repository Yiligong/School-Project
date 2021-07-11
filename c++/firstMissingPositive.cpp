/*Task Description*/

/*Write a function:
int solution(vector<int> &A);
that, given an array A of N integers, 
returns the smallest positive integer (greater than 0) that does not occur in A.
For example, given A = [1, 3, 6, 4, 1, 2], the function should return 5.
Given A = [1, 2, 3], the function should return 4.
Given A = [−1, −3], the function should return 1.*/

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

int solution(vector<int> &A)
{
    if (find(A.begin(), A.end(), 1) == A.end())
    {
        return 1;
    }
    int n = A.size();
    for (int i = 0; i < n; i++)
    {
        if (A[i] <= 0 || A[i] > n)
        {
            A[i] = 1;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (A[abs(A[i]) - 1] > 0)
        {
            int a = A[abs(A[i]) - 1];
            A[abs(A[i]) - 1] = -a;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (A[i] > 0)
        {
            return i + 1;
        }
    }
    return n + 1;
}

int main()
{
    vector<int> A{1, 3, 6, 4, 1, 2};
    cout << solution(A);
    cout << endl;
    return 0;
}