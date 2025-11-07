#include <iostream>
#include <string>
#include <list>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <map>
#include <algorithm>
#include <queue>
#include <SDL.h>
 
using namespace std; 

// Globals
const int screenHorizontal = 640;
const int screenVertical = 360;
const float spaceBetweenYMult = 10;
const float spaceBetweenXMult = 5.625;
list<SDL_Rect> renderLineList;

// space between points = (screenVertical/numberOfPoints)-pointHeight

struct Vector2 {
      float x;
      float y;
      Vector2(float X, float Y){
           x = X;
           y = Y;
      }
};

int HighestY(vector<Vector2> vv2){
	int highest = 0;
	for (int i = 0; i < vv2.size(); ++i){
		if (vv2[i].y > highest){highest = vv2[i].y;}
	}
	return highest;
}

int HighestX(vector<Vector2> vv2){
	int highest = 0;
	for (int i = 0; i < vv2.size(); ++i){
		if (vv2[i].x > highest){highest = vv2[i].x;}
	}
	return highest;
}

string printVector2(const Vector2 v){
	string str = "(";
	str += to_string(v.x);
	str += ",";
	str += to_string(v.y);
	str += ")";
	return str;
}

map<int, vector<Vector2>> generateXPlain(vector<Vector2> arr){
    map<int, vector<Vector2>> Xplain;
    for(auto coordinates : arr){
		Xplain[coordinates.x].push_back(coordinates);
    }
    return Xplain;
}

map<int, vector<Vector2>> generateYPlain(vector<Vector2> arr){
    map<int, vector<Vector2>> Yplain;
	for(auto coordinates : arr){
        Yplain[coordinates.y].push_back(coordinates);
    }
    return Yplain;
}

struct renderPointsData{
	float horizontalBorder;
	float verticalBorder;
	float YspaceBetweenPoints;
	float pointHeight;
	float XspaceBetweenPoints;
	float pointWidth;
};

renderPointsData gRPD;

renderPointsData renderPoints(SDL_Renderer* renderer, vector<Vector2> pointArray){
	renderPointsData rPD;
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	const float horizontalBorder = 0.0703125*screenHorizontal;
	rPD.horizontalBorder = horizontalBorder;
	
	const float verticalBorder = 0.125*screenVertical;
	rPD.verticalBorder = verticalBorder;
	
	const float YspaceBetweenPoints = (screenVertical-verticalBorder*2)/(HighestY(pointArray)+1);
	rPD.YspaceBetweenPoints = YspaceBetweenPoints;
	
	const float pointHeight = YspaceBetweenPoints/4;
	rPD.pointHeight = pointHeight;
	
	const float XspaceBetweenPoints = (screenHorizontal-horizontalBorder*2)/(HighestX(pointArray)+1);
	rPD.XspaceBetweenPoints = XspaceBetweenPoints;
	
	const float pointWidth = XspaceBetweenPoints/4;
	rPD.pointWidth = pointWidth;
	
	for (int i = 0; i < pointArray.size(); i++){
		SDL_Rect pointModel;
		pointModel.x = horizontalBorder + (pointArray[i].x * XspaceBetweenPoints) + XspaceBetweenPoints/4;
		pointModel.y = verticalBorder + (pointArray[i].y * YspaceBetweenPoints) + YspaceBetweenPoints/4;
		pointModel.w = pointWidth;
		pointModel.h = pointWidth;
		
		SDL_RenderFillRect(renderer, &pointModel);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	return rPD;
}

void XGenerateLineData( float xOrigin, float yOrigin, float xDestination, float yDestination, renderPointsData rPD){
	SDL_Rect rectangle;
	
	rectangle.x = rPD.horizontalBorder + (xOrigin * rPD.XspaceBetweenPoints) + rPD.pointWidth;
	const float highRightX =  rPD.horizontalBorder + (rPD.XspaceBetweenPoints/4) + (xDestination * rPD.XspaceBetweenPoints);
	
	rectangle.y = rPD.verticalBorder + (yOrigin * rPD.YspaceBetweenPoints) + (rPD.YspaceBetweenPoints/4) + rPD.pointHeight/1.5;
	const float lowLeftY = rectangle.y + rPD.pointHeight/2;
	
	rectangle.w = highRightX - rectangle.x;
	
	rectangle.h = lowLeftY - rectangle.y;
	
	renderLineList.push_back(rectangle);
}

void YGenerateLineData( float xOrigin, float yOrigin, float xDestination, float yDestination, renderPointsData rPD){
	SDL_Rect rectangle;
	
	rectangle.x = rPD.horizontalBorder + (xOrigin*rPD.XspaceBetweenPoints) + rPD.XspaceBetweenPoints/4 + rPD.pointWidth/3;
	const float highRightX = rectangle.x + rPD.pointHeight/2;
	
	rectangle.y = rPD.verticalBorder + (rPD.XspaceBetweenPoints/4) + (yOrigin * rPD.YspaceBetweenPoints) + rPD.pointHeight;
	const float lowLeftY = rPD.verticalBorder + (rPD.XspaceBetweenPoints/4) + (yDestination * rPD.YspaceBetweenPoints);
	
	rectangle.w = highRightX - rectangle.x;
	
	rectangle.h = lowLeftY - rectangle.y;
	
	renderLineList.push_back(rectangle);
}


void renderLineQueue(SDL_Renderer* renderer, list<SDL_Rect> lineToRenderList){
	if (!lineToRenderList.empty()){
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (auto i = lineToRenderList.begin(); i != lineToRenderList.end(); ++i){
			SDL_RenderFillRect(renderer, &(*i));
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}
	return;
}

void renderPoints(SDL_Renderer* renderer, vector<Vector2> pointArray, renderPointsData rPD){
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	for (int i = 0; i < pointArray.size(); i++){
		SDL_Rect pointModel;
		pointModel.x = rPD.horizontalBorder + (pointArray[i].x * rPD.XspaceBetweenPoints) + rPD.XspaceBetweenPoints/4;
		pointModel.y = rPD.verticalBorder + (pointArray[i].y * rPD.YspaceBetweenPoints) + rPD.YspaceBetweenPoints/4;
		pointModel.w = rPD.pointWidth;
		pointModel.h = rPD.pointWidth;
		SDL_RenderFillRect(renderer, &pointModel);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

mutex mtx;
condition_variable gate;
bool continueOperation = false;

vector<vector<Vector2>>FindRectangles (map<int, vector<Vector2>> xP, map<int, vector<Vector2>> yP, vector<vector<Vector2>> result){
	
	int index = 0;
	unique_lock<mutex> lock(mtx);
	// Iterates though each colum in the plain
	for (auto colum = xP.begin(); colum != xP.end(); colum++){
		renderLineList.clear();
		list<Vector2> toVisit;
		// Adds all colums items into queue
        for (auto i : colum->second){
			toVisit.push_back(i);
        }
		// Iterates though each item in the colum
        for (auto Ypoint : colum->second){
			toVisit.pop_front();
			renderLineList.clear();
			continueOperation = false;
			gate.wait(lock, []{return continueOperation;});
			cout << "Low left: " << printVector2(Ypoint) << "\n";
            if (toVisit.empty()){
                break;
			}
			// Iterates though each item in the same row as Ypoint
            for (auto xPoint : yP[Ypoint.y]){
				if (xPoint.x <= Ypoint.x){continue;}
				if (renderLineList.size() >= 4){
						renderLineList.clear();
						cout << "clearing \n";
						XGenerateLineData(Ypoint.x, Ypoint.y, xPoint.x, xPoint.y, gRPD);
				}
				continueOperation = false;
				cout << "low right: " << printVector2(xPoint) << "\n";
				XGenerateLineData(Ypoint.x, Ypoint.y, xPoint.x, xPoint.y, gRPD);
				gate.wait(lock, []{return continueOperation;});
				// Iterates though each point in the queue
				for (auto pointInColum : toVisit){
					if (renderLineList.size() >= 4){
						renderLineList.clear();
						cout << "clearing \n";
						XGenerateLineData(Ypoint.x, Ypoint.y, xPoint.x, xPoint.y, gRPD);
					}
					continueOperation = false;
					cout << "high left: " << printVector2(pointInColum) << "\n";
					YGenerateLineData(Ypoint.x, Ypoint.y, pointInColum.x, pointInColum.y, gRPD);
					gate.wait(lock, []{return continueOperation;});
					auto itr = yP[pointInColum.y].begin() + pointInColum.x + 1;
					// While element at the same x as xPoint isnt found
					while (itr->x < xPoint.x + 1){
						// If found, insert coordinates of the rectangle into output
						if (itr->x == xPoint.x){
							continueOperation = false;
							cout << "high right: " << printVector2(*itr) << "\n";
							XGenerateLineData(pointInColum.x, pointInColum.y, itr->x, itr->y, gRPD);
							gate.wait(lock, []{return continueOperation;});
							continueOperation = false;
							YGenerateLineData(xPoint.x, xPoint.y, itr->x, itr->y, gRPD);
							gate.wait(lock, []{return continueOperation;});
							
							result.push_back({Ypoint, xPoint, pointInColum, *itr});
							break;
						}
						continueOperation = false;
						cout << "Try high right: " << printVector2(*itr) << "\n";
						XGenerateLineData(pointInColum.x, pointInColum.y, itr->x, itr->y, gRPD);
						gate.wait(lock, []{return continueOperation;});
						itr++;
						continue;
					}
				}
			}
            
        }
		index++;
		if (index >= xP.size()-1){break;}
    }
	return result;
}

void ExecuteRectangleFunction(vector<Vector2> points, vector<vector<Vector2>>* result){
	auto xPlain = generateXPlain(points);
	auto yPlain = generateYPlain(points);
	*result = FindRectangles(xPlain, yPlain, {});
	renderLineList.clear();
	cout << "ExecutionTerminated \n";
	return;
}

 
int main(int argv, char** args){

	// Initialize window and renderer pointers
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	
	// Initialize windows
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(screenHorizontal, screenVertical, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "Rectangle Vizualization");
	
	// Set color and render
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderPresent(renderer);
	
	SDL_Event event;
	bool running = true;
	
	// Initialize the true stuff
	const Vector2 p1(0,0);
	const Vector2 p2(0,1);
	const Vector2 p3(0,2);
	const Vector2 p4(1,0);
	const Vector2 p5(1,1);
	const Vector2 p6(1,2);
	const Vector2 p7(2,0);
	const Vector2 p8(2,1);
	const Vector2 p9(2,2);
	
	const vector<Vector2> pointArray = {p1,p2,p3,p4,p5,p6,p7,p8,p9};
	gRPD = renderPoints(renderer, pointArray);
	
	// Initialize Algorithm
	vector<vector<Vector2>>* result = new vector<vector<Vector2>>;
	thread rectangleFunctionThread(ExecuteRectangleFunction, pointArray, result);
	
	while (running == true){
		renderPoints(renderer, pointArray, gRPD);
		renderLineQueue(renderer, renderLineList);
		while (SDL_PollEvent(&event)){
			switch (event.type){
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym){
						case SDLK_RIGHT:
							cout << "Right Arrow Pressed \n";
							continueOperation = true;
							gate.notify_one();
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;
			}
		}
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
	}
	
	
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	return 0;
}