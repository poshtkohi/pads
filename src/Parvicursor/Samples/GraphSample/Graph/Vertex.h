#ifndef __GraphSample_Vertex_h__
#define __GraphSample_Vertex_h__


#include "../../../Parvicursor/general.h"
#include "../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../Parvicursor/System/String/String.h"
#include "../../../Parvicursor/System.Collections/ArrayList/ArrayList.h"

using namespace System;
using namespace System::Collections;

//**************************************************************************************************************//

enum VertexColor
{
    WHITE,
    GRAY,
    BLACK
};

namespace GraphSample
{
		//----------------------------------------------------
		class Vertex : public Object
        {
			/*---------------------fields----------------*/
			private: Object *data;
			private: String *name;
			private: ArrayList *adjacency_list; // The adjacency list of the current vertex.
			private: bool disposed;
			private: VertexColor color; // The color of the current vertex that represents the recent explored status of this vertex.
			private: Int32 timestamp_d, timestamp_f;
			private: Vertex *predecessor;
			//private: ArrayList *weights;
			/*---------------------methods----------------*/
			public: Vertex();
			public: ~Vertex();
			public: Object *get_Data();
			public: void set_Data(Object *data); // Set the vertex data
			public: void set_Color(VertexColor color); // Set the vertex color.
			public: String *get_Name();
			public: void set_Name(String *name);
			public: VertexColor get_Color(); // Get the vertex color.
			public: Int32 get_timestamp_d();
			public: Int32 get_timestamp_f();
			public: void set_timestamp_d(Int32 time);
			public: void set_timestamp_f(Int32 time);
			public: void Add_AdjacentVertex(Vertex *vertex); // Add a vertex instance to the current vertex.
			public: void Remove_AdjacentVertex(Vertex *vertex); // Remove a vertex instance to current vertex.
			public: Int32 get_AdjacencyList_Count(); // Get the number of available slots in the adjacency_list.
			public: Vertex *get_AdjacentVertexAt(Int32 index); // Get an adjacent vertex at the specified index.
			public: Vertex *get_Predecessor();
			public: void set_Predecessor(Vertex *predecessor);
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

