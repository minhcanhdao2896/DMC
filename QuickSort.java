package exercise.exercise1;

import java.util.Arrays;

public class QuickSort extends sort {

	public QuickSort(int[] arr) {
		super(arr);
		// TODO Auto-generated constructor stub
		if(arr.length != 0)
		{
			arrayToSort = new int[arr.length];
			System.arraycopy(arr, 0 , arrayToSort, 0 , arr.length);
		}
	}
	
	private int partition(int arr[], int l, int h)
	{
		int pivot = arr[h];
		int i = l - 1;
		for(int j = l; j<=h-1;j++)
		{
			if(arr[j] <= pivot)
			{
				i++;
				swap(arr,i,j);
			}
		}
		swap(arr,i+1,h);
		return (i+1);
	}
	private void quick_sort(int arr[], int l, int h)
	{
		if(l < h)
		{
			int t = partition(arr,l,h);
			quick_sort(arr,l,t-1);
			quick_sort(arr,t+1,h);
		}
	}
	
	public void print_result_quick() {
 		if(arrayToSort.length != 0)
 		{
 			quick_sort(arrayToSort,0,arrayToSort.length-1);
 			System.out.println("Array after quick sort");
 			System.out.println(Arrays.toString(arrayToSort));
 		}
 	}
	
}


