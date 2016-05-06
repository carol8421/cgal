#ifndef SCENE_POLYHEDRON_SHORTEST_PATH_ITEM_H
#define SCENE_POLYHEDRON_SHORTEST_PATH_ITEM_H

#include "Scene_polyhedron_shortest_path_item_config.h"
#include "Scene_polyhedron_item_decorator.h"
#include <CGAL/Three/Scene_interface.h>
#include "Messages_interface.h"

#include "Polyhedron_type.h"
#include "Kernel_type.h"

#include "opengl_tools.h"

#include <QGLViewer/qglviewer.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMainWindow>
#include <QObject>

#include <string>
#include <list>

#ifndef Q_MOC_RUN
#include <CGAL/Surface_mesh_shortest_path.h>
#endif

#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>

#include <boost/current_function.hpp>

struct Scene_polyhedron_shortest_path_item_priv;

class SCENE_POLYHEDRON_SHORTEST_PATH_ITEM_EXPORT Scene_polyhedron_shortest_path_item : public Scene_polyhedron_item_decorator
{
  Q_OBJECT
  
  friend class Polyhedron_demo_shortest_path_plugin;
  
public:
  typedef CGAL::Three::Scene_interface::Bbox Bbox;
  
  typedef boost::property_map<Polyhedron, CGAL::vertex_point_t>::type VertexPointMap;
  
  typedef boost::graph_traits<Polyhedron> GraphTraits;
  typedef GraphTraits::face_descriptor face_descriptor;
  typedef GraphTraits::face_iterator face_iterator;
  
  typedef CGAL::Surface_mesh_shortest_path_traits<Kernel, Polyhedron> Surface_mesh_shortest_path_traits;
  typedef CGAL::Surface_mesh_shortest_path<Surface_mesh_shortest_path_traits> Surface_mesh_shortest_path;
  typedef Surface_mesh_shortest_path::Face_location Face_location;
  typedef CGAL::AABB_face_graph_triangle_primitive<Polyhedron, VertexPointMap> AABB_face_graph_primitive;
  typedef CGAL::AABB_traits<Kernel, AABB_face_graph_primitive> AABB_face_graph_traits;
  typedef CGAL::AABB_tree<AABB_face_graph_traits> AABB_face_graph_tree;
  
  typedef Surface_mesh_shortest_path_traits::Barycentric_coordinate Barycentric_coordinate;
  typedef Surface_mesh_shortest_path_traits::Construct_barycentric_coordinate Construct_barycentric_coordinate;
  typedef Surface_mesh_shortest_path_traits::Ray_3 Ray_3;
  typedef Surface_mesh_shortest_path_traits::Point_3 Point_3;
  typedef Surface_mesh_shortest_path_traits::FT FT;

  enum Selection_mode
  {
    INSERT_POINTS_MODE = 0,
    REMOVE_POINTS_MODE = 1,
    SHORTEST_PATH_MODE = 2
  };

  enum Primitives_mode
  {
    VERTEX_MODE = 0,
    EDGE_MODE = 1,
    FACE_MODE = 2
  };
  
public:

  Scene_polyhedron_shortest_path_item();
  Scene_polyhedron_shortest_path_item(Scene_polyhedron_item* polyhedronItem, CGAL::Three::Scene_interface* sceneInterface, Messages_interface* messages, QMainWindow* mainWindow);
  ~Scene_polyhedron_shortest_path_item();
  
  void set_selection_mode(Selection_mode mode);
  Selection_mode get_selection_mode() const;
  void set_primitives_mode(Primitives_mode mode);
  Primitives_mode get_primitives_mode() const;
  
  virtual bool supportsRenderingMode(RenderingMode m) const;
  using Scene_polyhedron_item_decorator::draw;
  virtual void draw(CGAL::Three::Viewer_interface*) const;
  // Points OpenGL drawing
  virtual void draw_points(CGAL::Three::Viewer_interface*) const;
  
  virtual Scene_polyhedron_shortest_path_item* clone() const;
  
  bool deferred_load(Scene_polyhedron_item* polyhedronItem, CGAL::Three::Scene_interface* sceneInterface, Messages_interface* messages, QMainWindow* mainWindow);
  virtual bool load(const std::string& file_name);
  virtual bool save(const std::string& file_name) const;
  
protected:
  void initialize(Scene_polyhedron_item* polyhedronItem, CGAL::Three::Scene_interface* sceneInterface, Messages_interface* messages, QMainWindow* mainWindow);
  void deinitialize();
  
  virtual bool isFinite() const;
  virtual bool isEmpty() const;
  virtual void compute_bbox()const;
  virtual QString toolTip() const;
  friend struct Scene_polyhedron_shortest_path_item_priv;
  Scene_polyhedron_shortest_path_item_priv* d;
  
protected:
  bool eventFilter(QObject* /*target*/, QEvent * gen_event);
  
public Q_SLOTS:
  virtual void poly_item_changed();
  virtual void invalidateOpenGLBuffers();
};

#endif // SCENE_POLYHEDRON_SHORTEST_PATH_ITEM_H
