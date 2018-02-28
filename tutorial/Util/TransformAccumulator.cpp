#include "TransformAccumulator.h"

osg::Matrix transformAccumulator::getMatrix()
{
   return mpcb->matrix;
}
transformAccumulator::transformAccumulator()
{
   parent = NULL;
   node = new osg::Node;
   mpcb = new updateAccumlatedMatrix();
   node->setUpdateCallback(mpcb);
}

bool transformAccumulator::attachToGroup(osg::Group* g)
{
   bool success = false;
   if (parent != NULL)
   {
      int n = parent->getNumChildren();
      for (int i = 0; i < n; i++)
      {
         if (node == parent->getChild(i) )
         {
            parent->removeChild(i,1);
            success = true;
         }
      }
      if (! success)
      {
         return success;
      }
   }
   g->addChild(node);   //hyy 把node（在本类中new产生）添加到到一个Group节点对象下,就可以
						//通过getMatrix()方法来获取场景中节点的世界坐标矩阵了
   return true;
}