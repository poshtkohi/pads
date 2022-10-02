//#include "stdafx.h"

#include "Vertex.h"

//**************************************************************************************************************//

namespace GraphSample
{
		//----------------------------------------------------
		Vertex::Vertex()
		{
			this->disposed = false;
			this->adjacency_list = null;
			this->name = null;
		}
		//----------------------------------------------------
		Vertex::~Vertex()
		{
			if(!disposed)
            {
				if(this->adjacency_list != null)
				{
					delete this->adjacency_list;
					this->adjacency_list = null;
				}
				this->disposed = true;
            }
		}
		//----------------------------------------------------
		Object *Vertex::get_Data()
		{
			return this->data;
		}
		//----------------------------------------------------
		void Vertex::set_Data(Object *data)
		{
			this->data = data;
		}
		//----------------------------------------------------
		String *Vertex::get_Name()
		{
			return this->name;
		}
		//----------------------------------------------------
		void Vertex::set_Name(String *name)
		{
			this->name = name;
		}
		//----------------------------------------------------
		void Vertex::set_Color(VertexColor color)
		{
			this->color = color;
		}
		//----------------------------------------------------
		VertexColor Vertex::get_Color()
		{
			return this->color;
		}
		//----------------------------------------------------
		Int32 Vertex::get_timestamp_d()
		{
			return this->timestamp_d;
		}
		//----------------------------------------------------
		Int32 Vertex::get_timestamp_f()
		{
			return this->timestamp_f;
		}
		//----------------------------------------------------
		void Vertex::set_timestamp_d(Int32 time)
		{
			this->timestamp_d = time;
		}
		//----------------------------------------------------
		void Vertex::set_timestamp_f(Int32 time)
		{
			this->timestamp_f = time;
		}
		//----------------------------------------------------
		void Vertex::Add_AdjacentVertex(Vertex *vertex)
		{
			if(this->adjacency_list == null)
				this->adjacency_list = new ArrayList();

			this->adjacency_list->Add(vertex);
		}
		//----------------------------------------------------
		void Vertex::Remove_AdjacentVertex(Vertex *vertex)
		{
			if(this->adjacency_list != null)
				this->adjacency_list->Remove(vertex);
		}
		//----------------------------------------------------
		Int32 Vertex::get_AdjacencyList_Count()
		{
			if(this->adjacency_list != null)
				return this->adjacency_list->get_Count();
			else
				return 0;
		}
		//----------------------------------------------------
		Vertex *Vertex::get_AdjacentVertexAt(Int32 index)
		{
			if(this->adjacency_list != null)
				return (Vertex *)this->adjacency_list->get_Value(index);

			return null;
		}
		//----------------------------------------------------
		Vertex *Vertex::get_Predecessor()
		{
			return this->predecessor;
		}
		//----------------------------------------------------
		void Vertex::set_Predecessor(Vertex *predecessor)
		{
			this->predecessor = predecessor;
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
