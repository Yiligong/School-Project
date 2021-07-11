/*Task Description*/

/*Given an array of integers nums and an integer target, 
return indices of the two numbers such that they add up to target.
You may assume that each input would have exactly one solution, 
and you may not use the same element twice.
You can return the answer in any order.*/

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

vector<int> twoSum(vector<int> &nums, int target)
{
    map<int, int> m;
    vector<int> result;
    for (int i = 0; i < sizeof(nums); i++)
    {
        int diff = target - nums[i];
        if (m.find(diff) != m.end())
        {
            result.push_back(m[diff]);
            result.push_back(i);
            return result;
        }
        else
            m.insert(pair<int, int>(nums[i], i));
    }
}

int main()
{
    vector<int> nums{2, 7, 11, 15};
    int target = 9;
    vector<int> r = twoSum(nums, target);
    cout << r[0] << " " << r[1];
    cout << endl;
    return 0;
}