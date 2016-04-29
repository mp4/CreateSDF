#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <queue>
#include <cmath>
#include <cfloat>
#include <Magick++.h>

static Magick::Image currentImage;
static Magick::Image outputImage;
const int divider = 32;//valve's initial divider was 64
    //inputs shoudl be a power of 2
const double threshold = 0.5;
const double interiorScale =  256;
const double exteriorScale = 256*3;
const int fixedBounds = 4096;
double findBlack(int x, int y)
{
    bool found = false;
    int step = 1;
    double bestDistance = DBL_MAX;
    int bounds = fixedBounds;
    while(!found)
    {
        for(int i =0; i < (step + 2);i++)
        {
            for(int j=0; j < (step +2); j++)
            {
                //make sure pixel is on edge
                if(i == 0 || j == 0)
                {
                    //make sure pixel is in bounds
                    if((x-((step + 1)/2) + i) >= 0 && (x - ((step + 1)/2) + i ) <= currentImage.size().width() && 
                        (y - ((step + 1)/2) + j) >= 0 && (y - ((step + 1)/2) + j) <= currentImage.size().height()
                    )
                    {
                        if( (static_cast<Magick::ColorGray>( currentImage.pixelColor(x - ((step + 1)/2) + i,y - ((step + 1)/2) + j))).shade() < threshold )
                        {
                            double resDistx = abs(((step + 1)/2) - i);
                            double resDisty = abs(((step + 1)/2) - j);
                            double curDist =sqrt(resDistx * resDistx + resDisty*resDisty)/ interiorScale;
                        
                            if(curDist < bestDistance)
                            {
                                bestDistance = curDist;
                                if(bounds ==  fixedBounds)
                                {
                                    //set the bounds 
                                    bounds = (step*71)/50;
                                }
                            }
                        }
                    }
                }
                else if(i == (step + 1) || j == (step + 1))
                {
                     if( (static_cast<Magick::ColorGray>( currentImage.pixelColor(x - ((step + 1)/2) + i,y - ((step + 1)/2) + j))).shade() < threshold )
                    {
                            double resDistx = abs(((step + 1)/2) - i);
                            double resDisty = abs(((step + 1)/2) - j);
                            double curDist =  sqrt(resDistx * resDistx + resDisty*resDisty)/ interiorScale;
                            
                            if(curDist < bestDistance)
                            {
                                bestDistance = curDist;
                                if(bounds ==  fixedBounds)
                                {
                                    //set the bounds 
                                    bounds = (step*71)/50;
                                }
                            }
                    }
                }
                
                if(i > 1 && j > 1 && i != (step +1) && j != (step + 1))
                {
                    j = step;
                }
            }
        }
        step++;
        if(step > interiorScale)
        {
            break;
        }
        if(step > bounds)
        {
            break;
        }
    }
    
    return bestDistance;
}
double findWhite(int x, int y)
{
    bool found = false;
    int step = 1;
    double bestDistance = DBL_MAX;
    int bounds = fixedBounds;
    while(!found)
    {
        for(int i =0; i < (step + 2);i++)
        {
            for(int j=0; j < (step +2); j++)
            {
                //make sure pixel is on edge
                if(i == 0 || j == 0)
                {
                    //make sure pixel is in bounds
                    if((x-((step + 1)/2) + i) >= 0 && (x - ((step + 1)/2) + i ) <= currentImage.size().width() && 
                        (y - ((step + 1)/2) + j) >= 0 && (y - ((step + 1)/2) + j) <= currentImage.size().height()
                    )
                    {
                        if((static_cast<Magick::ColorGray>( currentImage.pixelColor(x - ((step + 1)/2) + i,y - ((step + 1)/2) + j))).shade() > threshold )
                        {
                            double resDistx = abs(((step + 1)/2) - i);
                            double resDisty = abs(((step + 1)/2) - j);
                            double curDist = sqrt(resDistx * resDistx + resDisty*resDisty)/ interiorScale;
                        
                            if(curDist < bestDistance)
                            {
                                bestDistance = curDist;
                                if(bounds ==  fixedBounds)
                                {
                                    //set the bounds 
                                    bounds = (step*71)/50;
                                }
                            }
                        }
                    }
                }
                else if(i == (step + 1) || j == (step + 1))
                {
                     if( (static_cast<Magick::ColorGray>( currentImage.pixelColor(x - ((step + 1)/2) + i,y - ((step + 1)/2) + j))).shade() > threshold )
                    {
                            double resDistx = abs(((step + 1)/2) - i);
                            double resDisty = abs(((step + 1)/2) - j);
                            double curDist = sqrt(resDistx * resDistx + resDisty*resDisty)/ interiorScale;
                        
                            if(curDist < bestDistance)
                            {
                                bestDistance = curDist;
                                if(bounds ==  fixedBounds)
                                {
                                    //set the bounds 
                                    bounds = (step*71)/50;
                                }
                            }
                    }
                }
                //hopefully an optimization 
                if(i > 1 && j > 1 && i != (step +1) && j != (step + 1))
                {
                    j = step;
                }
            }
        }
        
        step++;
        if(step > exteriorScale)
        {
            break;
        }
        if(step > bounds)
        {
            break;
        }
    }
    
    return bestDistance;
}
double distance(int x, int y, Magick::Image img)
{
   auto curCol =  static_cast<Magick::ColorGray>(img.pixelColor(x*divider, y*divider));
   if(curCol.shade() > threshold)
   {
       return 0.5 +  findBlack(x*divider,y * divider);
    }
    else 
    {
        return 0.5 - findWhite(x*divider, y*divider);
    }
}

void init(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "wrong number of arguments usage: program source dest" << std::endl;
        //program source dest
    }
    
    currentImage.read(argv[1]);
    
    
    
    Magick::Geometry outSize;
    outSize.width(currentImage.size().width()/divider);
    outSize.height(currentImage.size().height()/divider);
    outputImage = Magick::Image(outSize, "black");
    outputImage.type(Magick::GrayscaleType);
    
    currentImage.type(Magick::GrayscaleType);

    
}
void process()
{
    
    for(int i =0; i < outputImage.size().height(); i++)
    {
        for(int j =0; j < outputImage.size().width();j++)
        {
            Magick::ColorGray currentPixCol(distance(j, i, currentImage));
            
            outputImage.pixelColor(j,i, currentPixCol);
        }
    }
    
}
void finalize(char * dest)
{
    outputImage.write(dest);
}

int main(int argc, char * argv[])
{
    Magick::InitializeMagick(*argv);
    init(argc, argv);
    
    //do the work here
    process();
    
    
    finalize(argv[2]);
    return 0;
}
