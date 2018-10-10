//Collision.h: contains collision code for use with SFML
//
//yes, this is a header file that includes implementation of code. sue me
//
#ifndef COLLISION_H
#define COLLISION_H

#include <cmath> //this was supposed to be easy
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <limits>

class Collision {

	public:
	struct ColData {
		bool didHit;
		sf::Vector2f POI;
		sf::Vector2f normal; //normal from shape face at POI
		//sf::Vector2f path; //path from shape to circle origin
	};
	static bool DEBUG_ENABLED;
	static sf::RenderWindow* DEBUG_WINDOW;
	constexpr static float DEBUG_POINT_RADIUS = 5.0f;
	
	//could be extended to support collisions between cShapes & cShapes
	//or of course circles and circles, but that's kinda trivial
	static bool isShapeCollidingWithCircle(sf::ConvexShape, sf::CircleShape, ColData&);
	static bool isPointInsideCorners(sf::Vector2f, sf::Vector2f, sf::Vector2f);
};

bool Collision::DEBUG_ENABLED = false;
sf::RenderWindow* Collision::DEBUG_WINDOW = NULL;

bool Collision::isPointInsideCorners(sf::Vector2f point, sf::Vector2f corner1, sf::Vector2f corner2){
	float minX = ((corner1.x < corner2.x) ? corner1.x : corner2.x) - 0.1f; //tolerance; needed for horizontal lines
	float maxX = ((corner1.x > corner2.x) ? corner1.x : corner2.x) + 0.1f; //tolerance; needed for horizontal lines
	float minY = ((corner1.y < corner2.y) ? corner1.y : corner2.y) - 0.1f; //tolerance; needed for horizontal lines
	float maxY = ((corner1.y > corner2.y) ? corner1.y : corner2.y) + 0.1f; //tolerance; needed for horizontal lines
	
	return (point.x < maxX && point.x > minX && point.y < maxY && point.y > minY);
}

bool Collision::isShapeCollidingWithCircle(sf::ConvexShape shape, sf::CircleShape circle, ColData &colData){
	//determine if the given circle is colliding with the given convex shape
	sf::Vector2f cPos = circle.getPosition();
	sf::Vector2f sPos = shape.getPosition();

	int N = shape.getPointCount();
	sf::Vector2f thisPoint;
	sf::Vector2f prevPoint;
	sf::Vector2f POI;
	bool foundFacing = false;
	
	
	if (DEBUG_ENABLED == true && DEBUG_WINDOW != NULL){
		/*
		sf::CircleShape shapeOrigin;
		shapeOrigin.setRadius(DEBUG_POINT_RADIUS);
		shapeOrigin.setFillColor(sf::Color::Blue);
		shapeOrigin.setOrigin(DEBUG_POINT_RADIUS, DEBUG_POINT_RADIUS);
		shapeOrigin.setPosition(sPos.x, sPos.y);
		*/
		sf::CircleShape circleOrigin;
		circleOrigin.setRadius(circle.getRadius());
		circleOrigin.setFillColor(sf::Color::Blue);
		circleOrigin.setOrigin(circle.getRadius(), circle.getRadius());
		circleOrigin.setPosition(cPos.x, cPos.y);
		
		//DEBUG_WINDOW->draw(shapeOrigin);
		DEBUG_WINDOW->draw(circleOrigin);
	}
	
	//currently rotating shapes used as terrain is not supported, so please don't.
	//for each pair of adjacent point in our shape:
	prevPoint = shape.getPoint(N-1)+sPos-shape.getOrigin();
	for (int i=0; i<N; i++){
		thisPoint = shape.getPoint(i) + sPos - shape.getOrigin();
		
		//find POI of: face btwn two points, and normal from face to circle.
		//formulas below are derived from basic algebra. it's y=mx+b up in here :fire:
		float slopeFace;
		if (thisPoint.x == prevPoint.x)
			slopeFace = std::numeric_limits<float>::max();
		else
			slopeFace = (thisPoint.y-prevPoint.y)/(thisPoint.x-prevPoint.x);
		if (slopeFace == 0.0f) slopeFace = 0.001f;
		float slopeNormal = -1.0f / slopeFace; //perpendicular
		
		float bFace = thisPoint.y - slopeFace * thisPoint.x; //y-intercepts
		float bNormal = cPos.y - slopeNormal * cPos.x;

		POI.x = (bFace-bNormal)/(slopeNormal-slopeFace); //this formula is adorable
		POI.y = slopeFace * POI.x + bFace;
		
		if (DEBUG_ENABLED == true && DEBUG_WINDOW != NULL){			
			//show points that form our lines, and the POI we've located.
			sf::CircleShape p1Circle;
			p1Circle.setRadius(DEBUG_POINT_RADIUS);
			p1Circle.setFillColor(sf::Color::Magenta);
			p1Circle.setOrigin(DEBUG_POINT_RADIUS, DEBUG_POINT_RADIUS);
			p1Circle.setPosition(thisPoint.x, thisPoint.y);
			DEBUG_WINDOW->draw(p1Circle);
			
			sf::CircleShape poiCircle;
			poiCircle.setRadius(DEBUG_POINT_RADIUS);
			poiCircle.setFillColor(sf::Color::Yellow);
			poiCircle.setOrigin(DEBUG_POINT_RADIUS, DEBUG_POINT_RADIUS);
			poiCircle.setPosition(POI.x, POI.y);
			DEBUG_WINDOW->draw(poiCircle);
			
			//plot our lines, NOT using the points above: checks our algebra.
			int screenX = (int)(DEBUG_WINDOW->getSize().x);
			int step = screenX;
			int N = screenX/step+1;	//+1 for point at zero
			sf::Vertex faceLine[N];
			sf::Vertex normalLine[N];
			for (int i=0; i<N; i++){
				float x = 1.0f*i*step;
				faceLine[i] = sf::Vertex(sf::Vector2f(x, slopeFace*x + bFace), sf::Color::Cyan); 
				normalLine[i] = sf::Vertex(sf::Vector2f(x, slopeNormal*x + bNormal), sf::Color::Yellow);
				DEBUG_WINDOW->draw(faceLine, N, sf::LineStrip);
				DEBUG_WINDOW->draw(normalLine, N, sf::LineStrip);
			}
		}
		
		//does this POI lie on the actual face (not just a projection of it)?
		if (isPointInsideCorners(POI, thisPoint, prevPoint)){
			//the moment of truth: is POI within the circle?
			float dist = hypot(POI.x-cPos.x, POI.y-cPos.y); //hypot is from c++11
			if (circle.getRadius() >= dist){
				colData.didHit = true; //TODO: circle and shape are overlapping a tiny bit
				colData.POI = POI;
				colData.normal = sf::Vector2f(cPos.x - POI.x, cPos.y - POI.y);
				return true;
			}
		}
		prevPoint = thisPoint;
	}
	//we failed to find a face the circle was intersecting
	return false;
}

#endif
