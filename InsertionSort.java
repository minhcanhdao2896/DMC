package exercise.exercise1;

import java.util.Arrays;

public class InsertionSort extends sort {

	public InsertionSort(int[] arr) {
		super(arr);
		// TODO Auto-generated constructor stub
		if(arr.length != 0)
		{
			arrayToSort = new int[arr.length];
			System.arraycopy(arr, 0 , arrayToSort, 0 , arr.length);
		}
	}

		public void insertionSort(int arr[], int n)
		{
			for(int i=1; i<n; i++)
			{
				int key = arr[i];
				int j = i - 1;
				/* Move elements of arr[0..i-1], that are 
	            greater than key, to one position ahead 
	            of their current position */
				while(j>=0 && arr[j] > key)
				{
					arr[j+1] = arr[j];
					j = j - 1;
				}
				arr[j+1] = key;
			}
		}
		
	public void print_result_insertion() {
		if(arrayToSort.length != 0)
		{
			insertionSort(arrayToSort,arrayToSort.length);
			System.out.println("Array after insertion sort");
			System.out.println(Arrays.toString(arrayToSort));
		}
	}
}
