#include <bits/stdc++.h>

using namespace std;

void quick_sort(vector<int>& nums, int left, int right) {
    if(left >= right) {
        return;
    } 
    //选择中间元素
    int pivot = left + (right - left) / 2;
    int i = left - 1;
    int j = right + 1;
    while(true) {
        //分区划分
        do {i++;} while(nums[i] < nums[pivot]);
        do {j--;} while(nums[j] > nums[pivot]);
        if(i >= j) break;
        swap(nums[i], nums[j]);
    }
    quick_sort(nums, left, j);
    quick_sort(nums, j + 1, right);
}

int main() {
    vector<int> nums = {1, 3, 2, 8, 6};
    quick_sort(nums, 0, nums.size() - 1);
    for(const auto num : nums) {
        cout << num << " ";
    }
    return 0;
}