using System;
//-----------------------------------------------------------
public class SimpleStack
{

	private class Node
	{
		public Node Previous;
		public Object Item;
	}

	private Node head;

	public SimpleStack() 
	{
		head = new Node();
	}

	public void Push(Object item) 
	{
		Node node = new Node();
		node.Item = item;
		/*node.Previous = head.Previous;
		head.Previous = node;*/

		node.Previous = head;
		head = node;
	}

	public Object Pop() 
	{
		/*Node node = head.Previous;
		if (node == null)
			return null;
		head.Previous = node.Previous;
		return node.Item;*/

		Node current = head;
		if(current == null)
			return null;

		Object Item = current.Item;
		head = current.Previous;

		return Item;
	}
}
//-----------------------------------------------------------
namespace CSharpStackSample
{
	class Class1
	{
		//-----------------------------------------------------------
		[STAThread]
		static void Main(string[] args)
		{
			SimpleStack stack = new SimpleStack();
			int n = 10;
			Console.WriteLine("Stack Push Process");
			for(int i = 0 ; i < n ; i++)
			{
				stack.Push(i);
				Console.WriteLine("i: {0}", i);
			}

			Console.WriteLine("Stack Pop Process");
			Object temp;
			while((temp = stack.Pop()) != null)
				Console.WriteLine("i: {0}", temp.ToString());
		}
		//-----------------------------------------------------------
	}
}
