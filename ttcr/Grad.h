//
//  Grad.h
//  ttcr
//
//  Created by Bernard Giroux on 2014-03-04.
//  Copyright (c) 2014 Bernard Giroux. All rights reserved.
//

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ttcr_Grad_h
#define ttcr_Grad_h

#include <cmath>
#include <set>

#include <Eigen/Dense>

#include "ttcr_t.h"
#include "Node.h"

namespace ttcr {
    
    // least-squares, first order
    template <typename T>
    class Grad2D_ls_fo {
    public:
        Grad2D_ls_fo() {}
        
        sxz<T> compute(const Node<T> &n0,
                       const Node<T> &n1,
                       const Node<T> &n2,
                       const size_t nt);
        
    private:
        sxz<T> g;
        Eigen::Matrix<T, 3, 2> A;
        Eigen::Matrix<T, 2, 1> x;
        Eigen::Matrix<T, 3, 1> b;
    };
    
    template <typename T>
    sxz<T> Grad2D_ls_fo<T>::compute(const Node<T> &n0,
                                    const Node<T> &n1,
                                    const Node<T> &n2,
                                    const size_t nt) {
        
        // find centroid of triangle
        sxz<T> cent = {(n0.getX()+n1.getX()+n2.getX())/static_cast<T>(3.),
            (n0.getZ()+n1.getZ()+n2.getZ())/static_cast<T>(3.)};
        
        // time at centroid is inverse distance weeighted
        T w = 1./sqrt( (n0.getX()-cent.x)*(n0.getX()-cent.x) + (n0.getZ()-cent.z)*(n0.getZ()-cent.z) );
        T t = w*n0.getTT(nt);
        T den = w;
        w = 1./sqrt( (n1.getX()-cent.x)*(n1.getX()-cent.x) + (n1.getZ()-cent.z)*(n1.getZ()-cent.z) );
        t += w*n1.getTT(nt);
        den += w;
        w = 1./sqrt( (n2.getX()-cent.x)*(n2.getX()-cent.x) + (n2.getZ()-cent.z)*(n2.getZ()-cent.z) );
        t += w*n2.getTT(nt);
        den += w;
        
        t /= den;
        
        A(0,0) = n0.getX() - cent.x;
        A(0,1) = n0.getZ() - cent.z;
        b(0,0) = t - n0.getTT(nt);
        
        A(1,0) = n1.getX() - cent.x;
        A(1,1) = n1.getZ() - cent.z;
        b(1,0) = t - n1.getTT(nt);
        
        A(2,0) = n2.getX() - cent.x;
        A(2,1) = n2.getZ() - cent.z;
        b(2,0) = t - n2.getTT(nt);
        
        // solve Ax = b with least squares
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        g.x = x[0];
        g.z = x[1];
        return g;
    }
    
    
    // least-squares, second order
    template <typename T, typename NODE>
    class Grad2D_ls_so {
    public:
        Grad2D_ls_so() {}
        
        sxz<T> compute(const std::set<NODE*> &nodes,
                       const size_t nt);
        
    private:
        sxz<T> g;
        Eigen::Matrix<T, Eigen::Dynamic, 5> A;
        Eigen::Matrix<T, 5, 1> x;
        Eigen::Matrix<T, Eigen::Dynamic, 1> b;
    };
    
    
    template <typename T, typename NODE>
    sxz<T> Grad2D_ls_so<T,NODE>::compute(const std::set<NODE*> &nodes,
                                         const size_t nt) {
        
        assert(nodes.size()>=5);
        
        sxz<T> cent = { 0.0, 0.0 };
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            cent.x += (*n)->getX();
            cent.z += (*n)->getZ();
        }
        T den = 1./nodes.size();
        cent.x *= den;
        cent.z *= den;
        
        T t = 0.0;
        den = 0.0;
        
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            T w = 1./sqrt(((*n)->getX()-cent.x)*((*n)->getX()-cent.x) +
                          ((*n)->getZ()-cent.z)*((*n)->getZ()-cent.z) );
            t += w*(*n)->getTT(nt);
            den += w;
        }
        t /= den;
        
        A.resize( nodes.size(), 5 );
        b.resize( nodes.size(), 1 );
        
        size_t i=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            T dx = (*n)->getX()-cent.x;
            T dz = (*n)->getZ()-cent.z;
            
            A(i,0) = dx;
            A(i,1) = dz;
            A(i,2) = dx*dx;
            A(i,3) = dz*dz;
            A(i,4) = dx*dz;
            
            b(i,0) = t - (*n)->getTT(nt);
            i++;
        }
        
        // solve Ax = b with least squares
//        x = (A.transpose() * A).ldlt().solve(A.transpose() * b);
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        //	Eigen::Matrix<T, Eigen::Dynamic, 1> e = b-A*x;
        
        g.x = x[0];
        g.z = x[1];
        
        return g;
    }
    
    template <typename T, typename NODE>
    class Grad3D {
    public:
        virtual ~Grad3D() {}
        virtual sxyz<T> compute(const sxyz<T> &pt, const std::set<NODE*> &nodes,
                                const size_t nt) = 0;
        virtual sxyz<T> compute(const std::set<NODE*> &nodes,
                                const size_t nt) = 0;
    };
    
    // least-squares, first order
    template <typename T, typename NODE>
    class Grad3D_ls_fo : public Grad3D<T, NODE> {
    public:
        Grad3D_ls_fo() {}
        
        sxyz<T> compute(const sxyz<T> &pt, const std::set<NODE*> &nodes,
                        const size_t nt);
        sxyz<T> compute(const std::set<NODE*> &nodes,
                        const size_t nt);
        
    private:
        sxyz<T> g;
        
        Eigen::Matrix<T, Eigen::Dynamic, 3> A;
        Eigen::Matrix<T, 3, 1> x;
        Eigen::Matrix<T, Eigen::Dynamic, 1> b;
    };
    
   
    template <typename T, typename NODE>
    sxyz<T> Grad3D_ls_fo<T,NODE>::compute(const sxyz<T> &pt,
                                          const std::set<NODE*> &nodes,
                                          const size_t nt) {
        
        assert(nodes.size()>=4);
        
        T t = 0.0;
        T den = 0.0;
        
        int remove = 0;
        std::vector<T> d( nodes.size() );
        size_t nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            d[nn] = sqrt(((*n)->getX()-pt.x)*((*n)->getX()-pt.x) +
                         ((*n)->getY()-pt.y)*((*n)->getY()-pt.y) +
                         ((*n)->getZ()-pt.z)*((*n)->getZ()-pt.z) );
            if ( d[nn] == 0.0 ) {
                remove++;
                nn++;
                continue;
            }
            T w = 1./d[nn];
            t += w*(*n)->getTT(nt);
            den += w;
            nn++;
        }
        t /= den;
        
        A.resize( nodes.size()-remove, 3 );
        b.resize( nodes.size()-remove, 1 );
        
        size_t i=0;
        nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            if ( d[nn] == 0.0 ) {
                nn++;
                continue;
            }
            A(i,0) = (*n)->getX()-pt.x;
            A(i,1) = (*n)->getY()-pt.y;
            A(i,2) = (*n)->getZ()-pt.z;
            
            b(i,0) = t - (*n)->getTT(nt);
            i++;
            nn++;
        }
        
        // solve Ax = b with least squares
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        //	Eigen::Matrix<T, Eigen::Dynamic, 1> e = b-A*x;
        
        //	if ( isnan(x[0]) || isnan(x[1]) || isnan(x[2]) ) {
        //		g.x = g.y = g.z = 0;
        //	}
        
        g.x = x[0];
        g.y = x[1];
        g.z = x[2];
        
        return g;
    }
    
    template <typename T, typename NODE>
    sxyz<T> Grad3D_ls_fo<T,NODE>::compute(const std::set<NODE*> &nodes,
                                          const size_t nt) {
        
        assert(nodes.size()>=4);
        
        sxyz<T> cent = { 0.0, 0.0, 0.0 };
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            cent.x += (*n)->getX();
            cent.y += (*n)->getY();
            cent.z += (*n)->getZ();
        }
        T den = 1./nodes.size();
        cent.x *= den;
        cent.y *= den;
        cent.z *= den;
        
        T t = 0.0;
        den = 0.0;
        
        int remove = 0;
        std::vector<T> d( nodes.size() );
        size_t nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            d[nn] = sqrt(((*n)->getX()-cent.x)*((*n)->getX()-cent.x) +
                         ((*n)->getY()-cent.y)*((*n)->getY()-cent.y) +
                         ((*n)->getZ()-cent.z)*((*n)->getZ()-cent.z) );
            if ( d[nn] == 0.0 ) {
                remove++;
                nn++;
                continue;
            }
            T w = 1./d[nn];
            t += w*(*n)->getTT(nt);
            den += w;
            nn++;
        }
        t /= den;
        
        A.resize( nodes.size()-remove, 3 );
        b.resize( nodes.size()-remove, 1 );
        
        size_t i=0;
        nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            if ( d[nn] == 0.0 ) {
                nn++;
                continue;
            }
            A(i,0) = (*n)->getX()-cent.x;
            A(i,1) = (*n)->getY()-cent.y;
            A(i,2) = (*n)->getZ()-cent.z;
            
            b(i,0) = t - (*n)->getTT(nt);
            i++;
            nn++;
        }
        
        // solve Ax = b with least squares
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        //	Eigen::Matrix<T, Eigen::Dynamic, 1> e = b-A*x;
        
        //	if ( isnan(x[0]) || isnan(x[1]) || isnan(x[2]) ) {
        //		g.x = g.y = g.z = 0;
        //	}
        
        g.x = x[0];
        g.y = x[1];
        g.z = x[2];
        
        return g;
    }
    
    
    // least-squares, second order
    template <typename T, typename NODE>
    class Grad3D_ls_so : public Grad3D<T, NODE> {
    public:
        Grad3D_ls_so() {}
        
        sxyz<T> compute(const sxyz<T> &pt, const std::set<NODE*> &nodes,
                        const size_t nt);
        sxyz<T> compute(const std::set<NODE*> &nodes,
                        const size_t nt);
        
    private:
        sxyz<T> g;
        Eigen::Matrix<T, Eigen::Dynamic, 9> A;
        Eigen::Matrix<T, 9, 1> x;
        Eigen::Matrix<T, Eigen::Dynamic, 1> b;
    };
    
    
    template <typename T, typename NODE>
    sxyz<T> Grad3D_ls_so<T,NODE>::compute(const sxyz<T> &pt,
                                          const std::set<NODE*> &nodes,
                                          const size_t nt) {
        // evaluate gradient are center of gravity
        assert(nodes.size()>=9);
        
        T t = 0.0;
        T den = 0.0;
        
        int remove = 0;
        std::vector<T> d( nodes.size() );
        size_t nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            d[nn] = sqrt(((*n)->getX()-pt.x)*((*n)->getX()-pt.x) +
                         ((*n)->getY()-pt.y)*((*n)->getY()-pt.y) +
                         ((*n)->getZ()-pt.z)*((*n)->getZ()-pt.z) );
            if ( d[nn] == 0.0 ) {
                remove++;
                nn++;
                continue;
            }
            T w = 1./d[nn];
            t += w*(*n)->getTT(nt);
            den += w;
            nn++;
        }
        t /= den;
        
        A.resize( nodes.size()-remove, 9 );
        b.resize( nodes.size()-remove, 1 );
        
        size_t i=0;
        nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            if ( d[nn] == 0.0 ) {
                nn++;
                continue;
            }
            T dx = (*n)->getX()-pt.x;
            T dy = (*n)->getY()-pt.y;
            T dz = (*n)->getZ()-pt.z;
            
            A(i,0) = dx;
            A(i,1) = dy;
            A(i,2) = dz;
            A(i,3) = 0.5*dx*dx;
            A(i,4) = 0.5*dy*dy;
            A(i,5) = 0.5*dz*dz;
            A(i,6) = dx*dy;
            A(i,7) = dx*dz;
            A(i,8) = dy*dz;
            
            b(i,0) = t - (*n)->getTT(nt);
            i++;
            nn++;
        }
        
        // solve Ax = b with least squares
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        //	Eigen::Matrix<T, Eigen::Dynamic, 1> e = b-A*x;
        
        g.x = x[0];
        g.y = x[1];
        g.z = x[2];
        
        return g;
    }
    
    template <typename T, typename NODE>
    sxyz<T> Grad3D_ls_so<T,NODE>::compute(const std::set<NODE*> &nodes,
                                          const size_t nt) {
        // evaluate gradient at center of gravity
        assert(nodes.size()>=9);
        
        sxyz<T> cent = { 0.0, 0.0, 0.0 };
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            cent.x += (*n)->getX();
            cent.y += (*n)->getY();
            cent.z += (*n)->getZ();
        }
        T den = 1./nodes.size();
        cent.x *= den;
        cent.y *= den;
        cent.z *= den;
        
        T t = 0.0;
        den = 0.0;
        
        int remove = 0;
        std::vector<T> d( nodes.size() );
        size_t nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            d[nn] = sqrt(((*n)->getX()-cent.x)*((*n)->getX()-cent.x) +
                         ((*n)->getY()-cent.y)*((*n)->getY()-cent.y) +
                         ((*n)->getZ()-cent.z)*((*n)->getZ()-cent.z) );
            if ( d[nn] == 0.0 ) {
                remove++;
                nn++;
                continue;
            }
            T w = 1./d[nn];
            t += w*(*n)->getTT(nt);
            den += w;
            nn++;
        }
        t /= den;
        
        A.resize( nodes.size()-remove, 9 );
        b.resize( nodes.size()-remove, 1 );
        
        size_t i=0;
        nn=0;
        for ( auto n=nodes.cbegin(); n!=nodes.cend(); ++n ) {
            if ( d[nn] == 0.0 ) {
                nn++;
                continue;
            }
            T dx = (*n)->getX()-cent.x;
            T dy = (*n)->getY()-cent.y;
            T dz = (*n)->getZ()-cent.z;
            
            A(i,0) = dx;
            A(i,1) = dy;
            A(i,2) = dz;
            A(i,3) = 0.5*dx*dx;
            A(i,4) = 0.5*dy*dy;
            A(i,5) = 0.5*dz*dz;
            A(i,6) = dx*dy;
            A(i,7) = dx*dz;
            A(i,8) = dy*dz;
            
            b(i,0) = t - (*n)->getTT(nt);
            i++;
            nn++;
        }
        
        // solve Ax = b with least squares
        x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);
        
        //	Eigen::Matrix<T, Eigen::Dynamic, 1> e = b-A*x;
        
        g.x = x[0];
        g.y = x[1];
        g.z = x[2];
        
        return g;
    }
    
}

#endif
