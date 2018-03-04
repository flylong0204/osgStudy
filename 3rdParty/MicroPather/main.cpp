#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

#include "micropather.h"
#include "../../Util/Axis.h"

struct MapItem
{
	int type; // 0 空白 1墙 2门
};

class CustomMap : public micropather::Graph
{
public:
	CustomMap() : mapXSize(0), mapYSize(0)
    {
        pather = new micropather::MicroPather(this);
    }
    
    virtual ~CustomMap()
    {
        delete pather;
    }
    
    void setMapData( const int* data, int x, int y )
    {
        _mapData.resize( x*y );
		for (unsigned int i = 0; i < _mapData.size(); ++i)
		{
			_mapData[i].type = *(data + i);
		}
        mapXSize = x;
        mapYSize = y;
    }
    
    void addDirectionCost( const osg::Vec2& dir, float weight )
    {
        DirectionCost cost;
        cost.direction = dir;
        cost.weight = weight;
        _costList.push_back( cost );
    }
    
    bool findPath( int startX, int startY, int endX, int endY, std::vector<osg::Vec2>& result )
    {
        float totalCost = 0.0f;
		micropather::MPVector<void*> path;
        int rtn = pather->Solve(XY2Node(startX, startY), XY2Node(endX, endY), &path, &totalCost );
        if ( rtn==micropather::MicroPather::SOLVED )
        {
            int x=0, y=0;
            result.resize( path.size() );
            for ( unsigned int i=0; i<path.size(); ++i )
            {
				Node2XY( path[i], x, y );
                result[i] = osg::Vec2((float)x, (float)y);
            }
            return true;
        }
        return false;
    }
    
    virtual float LeastCostEstimate( void* start, void* end )
    {
        int startX=0, startY=0, endX=0, endY=0;
		Node2XY( start, startX, startY );
		Node2XY( end, endX, endY );
        
        float dx = (float)(endX - startX); 
        float dy = (float)(endY - startY);
        return (float)sqrt(dx * dx + dy * dy);
    }
    
    virtual void AdjacentCost( void* node, micropather::MPVector<micropather::StateCost> * neighbors )
    {
        int currX=0, currY=0;
		Node2XY( node, currX, currY );
        for ( unsigned int i=0; i<_costList.size(); ++i )
        {
            int nx = currX + (int)_costList[i].direction.x();
            int ny = currY + (int)_costList[i].direction.y();
            if ( checkPassable(nx, ny) )
            {
                micropather::StateCost stateCost = { XY2Node(nx, ny), _costList[i].weight};
				neighbors->push_back(stateCost);
            }
        }
    }
    
    virtual void PrintStateInfo( void* node )
    {
        int currX=0, currY=0;
		Node2XY( node, currX, currY );
        std::cout << currX << " " << currY << std::endl;
    }
    
protected:
    bool checkPassable( int nx, int ny )
    {
        if ( nx >= 0 && nx < mapXSize && ny >= 0 && ny < mapYSize )
        {
            int data = _mapData[ny * mapXSize + nx].type;
            if ( data==0 ) return true;
        }
        return false;
    }
    
    void Node2XY( void* node, int& x, int& y )
    {
		intptr_t index = (intptr_t)node;
         y = index / mapXSize;
         x = index - y * mapXSize;
    }
    
    void* XY2Node( int x, int y )
    {
		return (void*)(y * mapXSize + x); 
	}
    
    struct DirectionCost
    {
        osg::Vec2 direction;
        float weight;
    };
    std::vector<DirectionCost> _costList;
    std::vector<MapItem> _mapData;
    micropather::MicroPather* pather;
	int mapXSize;
	int mapYSize;
	int xPos;
	int yPos;
};

osg::AnimationPathCallback* createAnimationPathCallback( osg::Vec3Array* vertices, float scale, float time )
{
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
    path->setLoopMode( osg::AnimationPath::LOOP );
    
    osg::Vec3 scaleFactor(scale, scale, scale);
    float delta_time = time / (float)vertices->size();
    for ( unsigned int i=0; i<vertices->size(); ++i )
    {
        osg::Quat rot; osg::Vec3 dir;
        if ( i<vertices->size()-1 )
        {
            dir = (*vertices)[i+1] - (*vertices)[i]; dir.normalize();
            rot.makeRotate( osg::X_AXIS, dir );
        }
        else
        {
            dir = (*vertices)[i] - (*vertices)[i-1]; dir.normalize();
            rot.makeRotate( osg::X_AXIS, dir );
        }
        path->insert( delta_time * (float)i, osg::AnimationPath::ControlPoint((*vertices)[i], rot, scaleFactor) );
    }
    
    osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    apcb->setAnimationPath( path.get() );
    return apcb.release();    
}

int main( int argc, char** argv )
{
    // Create path properties
    const int mapData[10 * 10] =
    { 
        1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 0, 1,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 1,
        1, 0, 0, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 1, 0, 1, 1, 1, 1, 1, 1, 1
    };
    
	CustomMap pathFinder;
    pathFinder.addDirectionCost( osg::Vec2(1.0f, 0.0f), 1.0f ); /*! 向右 */
    pathFinder.addDirectionCost( osg::Vec2(0.0f, -1.0f), 1.0f ); /*! 向后 */
    pathFinder.addDirectionCost( osg::Vec2(-1.0f, 0.0f), 1.0f ); /*! 向左 */
    pathFinder.addDirectionCost( osg::Vec2(0.0f, 1.0f), 1.0f ); /*! 向前 */
    pathFinder.setMapData( mapData, 10, 10 );
    
    // Create the map geometries
    osg::ref_ptr<osg::ShapeDrawable> groundShape = new osg::ShapeDrawable(
        new osg::Box(osg::Vec3(45.0f, 45.0f, 0.0f), 100.0f, 100.0f, 0.5f) );
    groundShape->setColor( osg::Vec4(0.2f, 0.2f, 0.2f, 0.2f) );
    
    osg::ref_ptr<osg::Geode> ground = new osg::Geode;
    ground->addDrawable( groundShape.get() );
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 5.0f), 10.0f)) );
    
    osg::ref_ptr<osg::MatrixTransform> scene = new osg::MatrixTransform;
    scene->addChild( ground.get() );
    for ( int x=0; x<10; ++x )
    {
        for ( int y=0; y<10; ++y )
        {
            if ( mapData[y*10 + x]==0 ) continue;
            osg::ref_ptr<osg::MatrixTransform> boxNode = new osg::MatrixTransform;
            boxNode->setMatrix( osg::Matrix::translate(10.0f*(float)x, 10.0f*(float)y, 0.0f) );
            boxNode->addChild( geode.get() );
            scene->addChild( boxNode.get() );
        }
    }
    
    // Compute and draw found path
    std::vector<osg::Vec2> result;
    if ( pathFinder.findPath(2, 0, 2, 9, result) )
    {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(result.size());
        for ( unsigned int i=0; i<result.size(); ++i )
            (*vertices)[i] = osg::Vec3( 10.0f*result[i].x(), 10.0f*result[i].y(), 2.0f );
        
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
        (*normals)[0] = osg::Vec3(0.0f, 0.0f, 1.0f);
        
        osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array(1);
        (*color)[0] = osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
        
        osg::ref_ptr<osg::Geometry> pathGeom = new osg::Geometry;
        pathGeom->setVertexArray( vertices.get() );
        pathGeom->setNormalArray( normals.get(), osg::Array::BIND_OVERALL );
        pathGeom->setColorArray( color.get(), osg::Array::BIND_OVERALL );
        pathGeom->addPrimitiveSet( new osg::DrawArrays(GL_LINE_STRIP, 0, result.size()) );
        
        osg::ref_ptr<osg::Geode> path = new osg::Geode;
        path->addDrawable( pathGeom.get() );
        scene->addChild( path.get() );
        
        osg::ref_ptr<osg::MatrixTransform> actor = new osg::MatrixTransform;
        actor->getOrCreateStateSet()->setMode( GL_NORMALIZE, osg::StateAttribute::ON );
        actor->addChild( osgDB::readNodeFile("../models/dumptruck.osgt") );
        actor->addUpdateCallback( createAnimationPathCallback(vertices.get(), 0.3f, 15.0f) );
        scene->addChild( actor.get() );
    }
	scene->addChild(Util::Axis::createAxis(100.0f, 100.0f, 100.0f));

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->setMatrix(osg::Matrix::rotate(osg::PI / 2.0f, osg::Vec3(1.0f, 0.0f, 0.0f)));
	mt->addChild(scene.get());

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );
    viewer.addEventHandler( new osgViewer::StatsHandler );
    viewer.addEventHandler( new osgViewer::WindowSizeHandler );
    //viewer.setSceneData( scene.get() );
	viewer.setSceneData(mt.get());
    return viewer.run();
}
