// Copyright (c) 2018 GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0+
//
//
// Author(s)     : Maxime Gimeno and Sebastien Loriot


#ifndef CGAL_RIGID_MESH_COLLISION_DETECTION_H
#define CGAL_RIGID_MESH_COLLISION_DETECTION_H

#include <CGAL/license/Polygon_mesh_processing/collision_detection.h>

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
// TODO: move AABB_do_intersect_transform_traits.h in PMP/internal
#include <CGAL/AABB_do_intersect_transform_traits.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/Side_of_triangle_mesh.h>


namespace CGAL {

template <class TriangleMesh, class Kernel>
class Rigid_mesh_collision_detection
{
  typedef CGAL::AABB_face_graph_triangle_primitive<TriangleMesh> Primitive;
  typedef CGAL::AABB_do_intersect_transform_traits<Kernel, Primitive> Traits;
  typedef CGAL::AABB_tree<Traits> Tree;
  typedef Side_of_triangle_mesh<TriangleMesh, Kernel, Default, Tree> Side_of_tm;


  std::vector<const TriangleMesh*> m_triangle_mesh_ptrs;
  // TODO: we probably want an option with external trees
  std::vector<Tree> m_aabb_trees;
  std::vector<bool> m_is_closed;

public:
  template <class MeshRange>
  Rigid_mesh_collision_detection(const MeshRange& triangle_meshes)
  {
    init(triangle_meshes);
  }

  template <class MeshRange>
  void init(const MeshRange& triangle_meshes)
  {
    m_triangle_mesh_ptrs.reserve( triangle_meshes.size() );
    m_aabb_trees.reserve( m_triangle_mesh_ptrs.size() );
    m_is_closed.resize(m_triangle_mesh_ptrs.size(), false);

    BOOST_FOREACH(const TriangleMesh& tm, triangle_meshes)
    {
      if (is_closed(tm))
        m_is_closed[m_triangle_mesh_ptrs.size()]=true;
      m_triangle_mesh_ptrs.push_back( &tm );
      m_aabb_trees.push_back( Tree(tm) );
      m_aabb_trees.back.insert(faces(tm).begin(), faces(tm).end(), tm);
    }
  }

  void set_transformation(std::size_t mesh_id, const Aff_transformation_3<Kernel>& aff_trans)
  {
    m_aabb_trees[mesh_id].traits().set_transformation(aff_trans);
  }

  std::vector<std::size_t>
  set_transformation_and_all_intersections(std::size_t mesh_id,
                                           const Aff_transformation_3<Kernel>& aff_trans,
                                           bool report_volume_inclusion_as_intersection)
  {
    set_transformation(mesh_id, aff_trans);
    std::vector<std::size_t> res;

    // TODO: use a non-naive version
    for(std::size_t k=0; k<m_aabb_trees.size(); ++k)
    {
      if(k==mesh_id) continue;
      // TODO: think about an alternative that is using a traversal traits
      if ( m_aabb_trees[k].do_intersect( m_aabb_trees[mesh_id] ) )
        res.push_back(k);
      else
      {
        if (report_volume_inclusion_as_intersection)
        {
          if (m_is_closed[mesh_id])
          {
            Side_of_tm side_of_mid(m_aabb_trees[mesh_id]);
            typename Kernel::Point_3 q = get(boost::vertex_point, *m_triangle_mesh_ptrs[k], *boost::begin(vertices(*m_triangle_mesh_ptrs[k])));
            if(side_of_mid(m_aabb_trees[k].traits.transformation()( q )) == CGAL::ON_BOUNDED_SIDE)
              res.push_back(k);
            continue;
          }
          if (m_is_closed[k])
          {
            Side_of_tm side_of_mk(m_aabb_trees[k]);
            typename Kernel::Point_3 q = get(boost::vertex_point, *m_triangle_mesh_ptrs[mesh_id], *boost::begin(vertices(*m_triangle_mesh_ptrs[mesh_id])));
            if(side_of_mk(m_aabb_trees.traits[mesh_id].transformation()( q )) == CGAL::ON_BOUNDED_SIDE)
              res.push_back(k);
          }
        }
      }
    }

    return res;
  }
};

} // end of CGAL namespace


#endif // CGAL_RIGID_MESH_COLLISION_DETECTION_H