// FileCreator.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Collections/ArrayList/ArrayList.h"

#include "Graph/Vertex.h"
#include "Graph/Graph.h"

//---------------------------------------
using namespace System;
using namespace System::Collections;
using namespace GraphSample;

//---------------------------------------
int main(int argc, char* argv[])
{
	Vertex *a = new Vertex();
	Vertex *b = new Vertex();
	Vertex *c = new Vertex();
	Vertex *d = new Vertex();

	String nameA = "a";
	String nameB = "b";
	String nameC = "c";
	String nameD = "d";

	a->set_Name(&nameA);
	a->Add_AdjacentVertex(b);
	a->Add_AdjacentVertex(c);
	a->Add_AdjacentVertex(d);

	b->set_Name(&nameB);
	b->Add_AdjacentVertex(a);
	b->Add_AdjacentVertex(c);
	b->Add_AdjacentVertex(d);

	c->set_Name(&nameC);
	c->Add_AdjacentVertex(a);
	c->Add_AdjacentVertex(b);
	c->Add_AdjacentVertex(d);

	d->set_Name(&nameD);
	d->Add_AdjacentVertex(a);
	d->Add_AdjacentVertex(b);
	d->Add_AdjacentVertex(c);


	Graph *graph = new Graph(Undirected);

	/*
	a->Add_AdjacentVertex(b);

	b->Add_AdjacentVertex(c);

	c->Add_AdjacentVertex(a);
	c->Add_AdjacentVertex(d);

	d->Add_AdjacentVertex(b);

	Graph *graph = new Graph(Directed);
*/

	graph->Add_Vertex(a);
	graph->Add_Vertex(b);
	graph->Add_Vertex(c);
	graph->Add_Vertex(d);

	graph->DFS();

	if(graph->has_Cycle())
		cout << "The graph has at least one cycle.\nThe number of fundamental closed loops are: " << graph->get_Cycles() << endl;
	else
		cout << "The graph has no cycle." << endl;

	graph->Remove_Vertex(d);
	graph->Print();

	delete a;
	delete b;
	delete c;
	delete d;
	delete graph;

	return 0;
}
//---------------------------------------
