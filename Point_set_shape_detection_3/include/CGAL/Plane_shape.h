#ifndef CGAL_SHAPE_DETECTION_3_PLANE_SHAPE_H
#define CGAL_SHAPE_DETECTION_3_PLANE_SHAPE_H

#include "Shape_base.h"
#include <set>

/*!
 \file Plane_shape.h
 */
namespace CGAL {
    /*!
     \brief Plane_shape implements Shape_base. The plane is parameterized by the normal vector and the distance to the origin.
     */
  template <class Sd_traits>
  class Plane_shape : public Shape_base<Sd_traits> {
  public:

    typedef typename Sd_traits::inputIterator inputIterator;///< random access iterator for input data.
    typedef typename Sd_traits::Geom_Traits::FT FT;///< number type.
    typedef typename Sd_traits::Geom_Traits::Point_3 Point;///< point type.
    typedef typename Sd_traits::Geom_Traits::Vector_3 Vector;///< vector type.
    typedef typename Sd_traits::Geom_Traits::Plane_3 Plane_3;///< plane type for conversion operator.
    typedef typename Sd_traits::PointPMap PointPMap;  ///< property map to access the location of an input point.
      typedef typename Sd_traits::NormalPMap NormalPMap; ///< property map to access the unoriented normal of an input point.


  public:
    Plane_shape() : Shape_base<Sd_traits>() {}
      /*!
       Helper function to write the plane equation and number of assigned points into a string.
       */
    std::string info() const {
      std::stringstream sstr;
      sstr << "Type: plane (" << m_normal.x() << ", " << m_normal.y() << ", " << m_normal.z() << ")x - " << m_d << "= 0" << " #Pts: " << this->m_indices.size();

      return sstr.str();
    }
      /*!
       Conversion operator to Plane_3 type.
       */

    operator Plane_3() const {
      return m_plane;
    }
      
      /*!
       Distance of the plane to the origin.
       */
    FT distance() const {
      return m_d;
    }
      
      /*!
       Normal vector of the plane.
       */
    Vector normal() const {
      return m_normal;
    }

  protected:
      /// \cond SKIP_IN_MANUAL
    virtual void create_shape(const std::vector<int> &indices) {
      Point p1 = get(this->m_pointPMap, *(this->m_first + indices[0]));
      Point p2 = get(this->m_pointPMap, *(this->m_first + indices[1]));
      Point p3 = get(this->m_pointPMap, *(this->m_first + indices[2]));

      m_plane = Plane_3(p1, p2, p3);

      m_normal = m_plane.orthogonal_vector();
      m_normal = m_normal * (1.0 / sqrt(m_normal.squared_length()));
      m_d = p1[0] * m_normal[0] + p1[1] * m_normal[1] + p1[2] * m_normal[2];

      //check deviation of the 3 normal
      Vector l_v;
      for (int i = 0;i<3;i++) {
        l_v = get(this->m_normalPMap, *(this->m_first + indices[i]));

        if (abs(l_v * m_normal) < this->m_normal_threshold * sqrt(l_v.squared_length())) {
          this->m_isValid = false;
          return;
        }

        m_point_on_primitive = p1;
        m_base1 = m_plane.base1();
        m_base1 = m_base1 * (1.0 / sqrt(m_base1.squared_length()));

        m_base2 = m_plane.base2();
        m_base2 = m_base2 * (1.0 / sqrt(m_base2.squared_length()));
      }
    }

    void parameters(std::vector<std::pair<FT, FT> > &parameterSpace, const std::vector<int> &indices, FT min[2], FT max[2]) const {
      Vector p = (get(this->m_pointPMap, *(this->m_first + indices[0])) - m_point_on_primitive);
      FT u = p * m_base1;
      FT v = p * m_base2;
      parameterSpace[0] = std::pair<FT, FT>(u, v);
      min[0] = max[0] = u;
      min[1] = max[1] = v;

      for (unsigned int i = 1;i<indices.size();i++) {
        Vector p = (get(this->m_pointPMap, *(this->m_first + indices[i])) - m_point_on_primitive);
        FT u = p * m_base1;
        FT v = p * m_base2;
        min[0] = (std::min<FT>)(min[0], u);
        max[0] = (std::max<FT>)(max[0], u);
        min[1] = (std::min<FT>)(min[1], v);
        max[1] = (std::max<FT>)(max[1], v);
        parameterSpace[i] = std::pair<FT, FT>(u, v);
      }
    }

    void parameter_extend(const Point &center, FT width, FT min[2], FT max[2]) const {
      min[0] = min[1] = max[0] = max[1] = 0;
      Point corner[8];
      corner[0] = center + Vector(width, width, width);
      corner[1] = center + Vector(-width, width, width);
      corner[2] = center + Vector(width, -width, width);
      corner[3] = center + Vector(-width, -width, width);
      corner[4] = center + Vector(width, width, -width);
      corner[5] = center + Vector(-width, width, -width);
      corner[6] = center + Vector(width, -width, -width);
      corner[7] = center + Vector(-width, -width, -width);

      for (unsigned int i = 0;i<8;i++) {
        Vector p = (corner[i] - m_plane.point());
        FT u = p * m_base1;
        FT v = p * m_base2;
        min[0] = (std::min)(min[0], u);
        min[1] = (std::min)(min[1], v);
        max[0] = (std::max)(max[0], u);
        max[1] = (std::max)(max[1], v);
      }
    }

    FT squared_distance(const Point &_p) const {
      return CGAL::squared_distance (m_plane, _p);
    }

    FT distance(const Point &_p) const {
      return m_normal[0] * _p[0] + m_normal[1] * _p[1] + m_normal[2] * _p[2] - m_d;
    }

    void squared_distance(std::vector<FT> &dists, const std::vector<int> &shapeIndex, const std::vector<unsigned int> &indices) {
      for (unsigned int i = 0;i<indices.size();i++) {
        if (shapeIndex[indices[i]] == -1)
          dists[i] = CGAL::squared_distance(m_plane, get(this->m_pointPMap, *(this->m_first + indices[i])));
      }
    }

    void cos_to_normal(std::vector<FT> &angles, const std::vector<int> &shapeIndex, const std::vector<unsigned int> &indices) const {
      for (unsigned int i = 0;i<indices.size();i++) {
        if (shapeIndex[indices[i]] == -1)
          angles[i] = abs(get(this->m_normalPMap, *(this->m_first + indices[i])) * m_normal);
      }
    }

    FT cos_to_normal(const Point &_p, const Vector &_n) const{
      return abs(_n * m_normal);
    } 
    
    virtual int required_samples() const {
      return 3;
    }

    virtual bool supports_connected_component() const {
      return true;
    }

    virtual bool wraps_u() const {
      return false;
    }

    virtual bool wraps_v() const {
      return false;
    }

  private:
    Plane_3	m_plane;
    Point m_point_on_primitive;
    Vector m_base1, m_base2, m_normal;
    FT m_d;
    /// \endcond
  };
}
#endif
