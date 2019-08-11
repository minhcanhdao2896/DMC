package exercise.exercise1;

//import java.util.Arrays;

public class sort {
	protected int arr[] ={2,1,9,7,100,125,5,985};
	protected int[] arrayToSort;
	
	public sort(int[] array) {
		if(arr.length != 0)
		{
			arrayToSort = new int[arr.length];
			System.arraycopy(arr, 0 , arrayToSort, 0 , arr.length);
			
			 System.out.println("Array Before Sort"); 
			
		    for(int i=0; i < arr.length; i++){ 
		    	
		             System.out.print(arr[i] + "  ");  
		     } 
		     System.out.println(); 
		}
	}
	public void swap(int array[],int i, int j)
	{
		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}

}


