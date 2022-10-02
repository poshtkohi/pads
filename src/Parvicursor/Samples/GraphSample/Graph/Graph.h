#ifndef __GraphSample_Graph_h__
#define __GraphSample_Graph_h__


#include "../../../Parvicursor/general.h"
#include "../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../Parvicursor/System/String/String.h"
#include "../../../Parvicursor/System.Collections/ArrayList/ArrayList.h"

#include "Vertex.h"

using namespace System;
using namespace System::Collections;

//**************************************************************************************************************//

enum GraphType
{
    Directed,
    Undirected,
};

namespace GraphSample
{
		//----------------------------------------------------
		class Graph : public Object
        {
			/*---------------------fields----------------*/
			private: ArrayList *vertices; // The vertices of the graph.
			private: bool disposed;
			private: Int32 time;
			private: Int32 cycles;
			private: bool has_cycle;
			private: GraphType type; // The type of the graph instance.
			//private: ArrayList *weights;
			/*---------------------methods----------------*/
			public: Graph(GraphType type);
			public: ~Graph();
			public: void Add_Vertex(Vertex *vertex); // Add a vertex to the graph.
			public: void Remove_Vertex(Vertex *vertex); // Remove a vertex from the graph.
			public: void DFS(); // Do the depth-first search algorithm.
			private: void DFS_visit(Vertex *u); // Perform the recursive traversal of the graph from the vertex u.
			public: Int32 get_Cycles();
			public: bool has_Cycle(); // Get whether the graph contains at least one cycle.
			public: void Print(); // Print the graph based on its  adjacency list representation.
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

