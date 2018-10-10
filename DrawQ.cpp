//DrawQ.cpp
//used to keep track of all Drawables, and their sorting order
#include <vector>

namespace DrawLayers{
	const int NUM_LAYERS = 5;
	enum Layer {
		background, backstage, stage, foreground, HUD
	};
}

class DrawQ{
	private:
	struct entry {
		DrawLayers::Layer layer;
		int order;
		sf::Drawable* drawable = NULL;
		entry* next = NULL;
	};
	entry* layerHeads[DrawLayers::NUM_LAYERS] = {NULL};
	
	public:
	void add(sf::Drawable&, DrawLayers::Layer, int);
	void remove(sf::Drawable*);
	void drawToWindow(sf::RenderWindow&);
};


void DrawQ::add(sf::Drawable &drawable, DrawLayers::Layer layer, int order=0){
	entry* newEntry = new entry;
	newEntry->layer = layer;
	newEntry->order = order;
	newEntry->drawable = &drawable;
	
	entry* currentEntry;
	entry* prevEntry = NULL;
	
	//if this is the first entry added, make it head
	if (layerHeads[layer] == NULL){
		layerHeads[layer] = newEntry;
		return;
	}
	
	//if we should become the new head, based on sorting order
	if (layerHeads[layer]->order > order){
		newEntry->next = layerHeads[layer];
		layerHeads[layer] = newEntry;
		return;
	}
	
	//find a place in the middle or at the end of the queue
	prevEntry = layerHeads[layer];
	currentEntry = prevEntry->next;
	while(currentEntry != NULL){
		if (currentEntry->order > order){
			//insert new entry at this position
			break;
		}
		prevEntry = currentEntry;
		currentEntry = currentEntry->next;
	}
	newEntry->next = currentEntry;
	prevEntry->next = newEntry;
}

void DrawQ::remove(sf::Drawable* drawable){
	//not currently supported :)
	//no good solution with current implementation
}

void DrawQ::drawToWindow(sf::RenderWindow& window){
	for (int l=0; l<DrawLayers::NUM_LAYERS; l++){
		//for each layer, starting with the back and heading forward
		entry* currentEntry = layerHeads[l];
		while(currentEntry != NULL){
			window.draw(*(currentEntry->drawable)); //I'm bad with pointers/reference for now
			currentEntry = currentEntry->next;
		}		
	}
}