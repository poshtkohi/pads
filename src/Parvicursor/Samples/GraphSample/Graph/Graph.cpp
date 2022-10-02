//#include "stdafx.h"

#include "Graph.h"

//**************************************************************************************************************//

namespace GraphSample
{
		//----------------------------------------------------
		Graph::Graph(GraphType type)
		{
			this->disposed = false;
			this->vertices = null;
			this->type = type;
		}
		//----------------------------------------------------
		Graph::~Graph()
		{
			if(!disposed)
            {
				if(this->vertices != null)
				{
					delete this->vertices;
					this->vertices = null;
				}
				this->disposed = true;
            }
		}
		//----------------------------------------------------
		void Graph::Add_Vertex(Vertex *vertex)
		{
			if(this->vertices == null)
				this->vertices = new ArrayList();

			this->vertices->Add(vertex);
		}
		//----------------------------------------------------
		void Graph::Remove_Vertex(Vertex *vertex)
		{
			if(this->vertices != null)
			{
				for(Int32 i = 0 ; i < this->vertices->get_Count() ; i++)
				{
					Vertex *vParent = (Vertex *)this->vertices->get_Value(i);

					Int32 repeats = 0;

					for(Int32 j = 0 ; j < vParent->get_AdjacencyList_Count() ; j++) // Count the number of repeated edges.
					{
						Vertex *v = (Vertex *)vParent->get_AdjacentVertexAt(j);
						if(v == vertex)
							repeats++;
					}

					for(Int32 k = 0 ; k < repeats ; k++)
						vParent->Remove_AdjacentVertex(vertex);

				}
				this->vertices->Remove(vertex);
			}
		}
		//----------------------------------------------------
		void Graph::DFS()
		{
			if(this->vertices != null)
			{
				this->cycles = 0;
				this->has_cycle = false;

				for(Int32 i = 0 ; i < this->vertices->get_Count() ; i++)
				{
					Vertex *u = (Vertex *)this->vertices->get_Value(i);
					u->set_Color(WHITE);
					u->set_Predecessor(null);
				}

				this->time = 0;

				for(Int32 i = 0 ; i < this->vertices->get_Count() ; i++)
				{
					Vertex *u = (Vertex *)this->vertices->get_Value(i);

					if(u->get_Color() == WHITE)
						this->DFS_visit(u);
				}
			}
		}
		//----------------------------------------------------
		void Graph::DFS_visit(Vertex *u)
		{
			u->set_Color(GRAY);
			this->time++;
			u->set_timestamp_d(this->time);

			for(Int32 i = 0 ; i < u->get_AdjacencyList_Count() ; i++)
			{
				Vertex *v = u->get_AdjacentVertexAt(i);

				if(this->type == Undirected && v->get_Color() == BLACK) // for undirected graph
				{
					this->cycles++; // detects the fundamental closed paths (closed loops in circuit theory or closed walks) in the Graph G
					this->has_cycle = true;
				}

				if(this->type == Directed && v->get_Color() == GRAY) // for directed graph (dgraph)
				{
					this->cycles++; // detects the fundamental closed paths (closed loops in circuit theory or closed walks) in the Graph G
					this->has_cycle = true;
				}

				if(v->get_Color() == WHITE)
				{
					v->set_Predecessor(u);
					DFS_visit(v);
				}
			}

			u->set_Color(BLACK);
			u->set_timestamp_f(this->time);
		}
		//----------------------------------------------------
		Int32 Graph::get_Cycles()
		{
			return this->cycles;
		}
		//----------------------------------------------------
		bool Graph::has_Cycle()
		{
			return this->has_cycle;
		}
		//----------------------------------------------------
		void Graph::Print()
		{
			if(this->vertices != null)
			{
				for(Int32 i = 0 ; i < this->vertices->get_Count() ; i++)
				{
					Vertex *vParent = (Vertex *)this->vertices->get_Value(i);
					if(vParent->get_Name() != null)
						cout << vParent->get_Name()->get_BaseStream() << ": ";

					for(Int32 j = 0 ; j < vParent->get_AdjacencyList_Count() ; j++)
					{
						Vertex *v = (Vertex *)vParent->get_AdjacentVertexAt(j);
						if(v->get_Name() != null)
							cout << v->get_Name()->get_BaseStream() << "\t";
					}

					cout << endl;
				}
			}
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
