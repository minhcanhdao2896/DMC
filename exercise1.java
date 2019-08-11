package exercise.exercise1;

public class exercise1 {
  private static exercise1 instance;
  private exercise1(){
	  
  }
  public static exercise1 getInstance(){
	  if (instance ==null){
		  instance= new exercise1();	  
	  }
	  return instance;
  }

public void run(String[] args){
	// int arr[] ={2,1,8,7,10,12,5,98};
	 
	 int arr[] ={}; 
	 
     BubbleSort s1=new BubbleSort(arr);
     s1.print_result_bubble();
     
     QuickSort s2= new QuickSort(arr);
     s2.print_result_quick();
     
     MergeSort s3= new MergeSort(arr);
     s3.print_result_merge();
     
     HeapSort s4= new HeapSort(arr);
     s4.print_result_heap();
     
     InsertionSort s5= new InsertionSort(arr);
     s5.print_result_insertion();
  
}
}
