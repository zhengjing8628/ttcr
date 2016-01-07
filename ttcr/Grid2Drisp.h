//
//  Grid2Drisp.h
//  ttcr
//
//  Created by Bernard Giroux on 2015-09-22.
//  Copyright © 2015 Bernard Giroux. All rights reserved.
//

#ifndef Grid2Drisp_h
#define Grid2Drisp_h

#include "Grid2Dri.h"
#include "Node2Disp.h"

template<typename T1, typename T2>
class Grid2Drisp : public Grid2Dri<T1,T2,Node2Disp<T1,T2>> {
public:
    Grid2Drisp(const T2 nx, const T2 nz, const T1 ddx, const T1 ddz,
               const T1 minx, const T1 minz, const T2 nnx, const T2 nnz,
               const size_t nt=1);
    
    virtual ~Grid2Drisp() {
    }
    
    int setSlowness(const std::vector<T1>& s) {
        if ( nPrimary != s.size() ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        size_t nn=0;
        for ( size_t n=0; n<this->nodes.size(); ++n ) {
            if ( this->nodes[n].getPrimary() == 5 )
                this->nodes[n].setNodeSlowness( s[nn++] );
        }
        interpSlownessSecondary();
        return 0;
    }
    

    int raytrace(const std::vector<sxz<T1>>& Tx,
                 const std::vector<T1>& t0,
                 const std::vector<sxz<T1>>& Rx,
                 std::vector<T1>& traveltimes,
                 const size_t threadNo=0) const;
    
    int raytrace(const std::vector<sxz<T1>>& Tx,
                 const std::vector<T1>& t0,
                 const std::vector<const std::vector<sxz<T1>>*>& Rx,
                 std::vector<std::vector<T1>*>& traveltimes,
                 const size_t threadNo=0) const;
    
    int raytrace(const std::vector<sxz<T1>>& Tx,
                 const std::vector<T1>& t0,
                 const std::vector<sxz<T1>>& Rx,
                 std::vector<T1>& traveltimes,
                 std::vector<std::vector<sxz<T1>>>& r_data,
                 const size_t threadNo=0) const;
    
    int raytrace(const std::vector<sxz<T1>>& Tx,
                 const std::vector<T1>& t0,
                 const std::vector<const std::vector<sxz<T1>>*>& Rx,
                 std::vector<std::vector<T1>*>& traveltimes,
                 std::vector<std::vector<std::vector<sxz<T1>>>*>& r_data,
                 const size_t threadNo=0) const;
    
    int raytrace(const std::vector<sxz<T1>>& Tx,
                 const std::vector<T1>& t0,
                 const std::vector<sxz<T1>>& Rx,
                 std::vector<T1>& traveltimes,
                 std::vector<std::vector<sxz<double>>>& r_data,
                 std::vector<std::vector<siv<double>>>& l_data,
                 const size_t threadNo=0) const;
    
protected:
    T2 nsnx;    // number of secondary nodes in x
    T2 nsnz;    // number of secondary nodes in z
    T2 nsgx;    // number of subgrid cells in x
    T2 nsgz;    // number of subgrid cells in z
    T2 nPrimary;
    
    void buildGridNodes();
    
    void interpSlownessSecondary();
    
    void propagate(std::priority_queue<Node2Disp<T1,T2>*,
                   std::vector<Node2Disp<T1,T2>*>,
                   CompareNodePtr<T1>>& queue,
                   std::vector<bool>& inQueue,
                   std::vector<bool>& frozen,
                   const size_t threadNo) const;
    
    void initQueue(const std::vector<sxz<T1>>& Tx,
                   const std::vector<T1>& t0,
                   std::priority_queue<Node2Disp<T1,T2>*,
                   std::vector<Node2Disp<T1,T2>*>,
                   CompareNodePtr<T1>>& queue,
                   std::vector<Node2Disp<T1,T2>>& txNodes,
                   std::vector<bool>& inQueue,
                   std::vector<bool>& frozen,
                   const size_t threadNo) const;
    
private:
    Grid2Drisp() {}
    Grid2Drisp(const Grid2Drisp<T1,T2>& g) {}
    Grid2Drisp<T1,T2>& operator=(const Grid2Drisp<T1,T2>& g) {}
    
};

template<typename T1, typename T2>
Grid2Drisp<T1,T2>::Grid2Drisp(const T2 nx, const T2 nz, const T1 ddx, const T1 ddz,
                              const T1 minx, const T1 minz, const T2 nnx, const T2 nnz,
                              const size_t nt) :
Grid2Dri<T1,T2,Node2Disp<T1,T2>>(nx,nz,ddx,ddz,minx,minz,nt),
nsnx(nnx), nsnz(nnz), nsgx(0), nsgz(0),
nPrimary((nx+1) * (nz+1))
{
    buildGridNodes();
    this->buildGridNeighbors();
}

template<typename T1, typename T2>
void Grid2Drisp<T1,T2>::buildGridNodes() {
    
    this->nodes.resize( // noeuds secondaires
                       this->nCellx*nsnx*(this->nCellz+1) +
                       this->nCellz*nsnz*(this->nCellx+1) +
                       // noeuds primaires
                       (this->nCellx+1) * (this->nCellz+1),
                       Node2Disp<T1,T2>(this->nThreads));

    
    T1 dxs = this->dx/(nsnx+1);
    T1 dzs = this->dz/(nsnz+1);
    
    T2 cell_upLeft = std::numeric_limits<T2>::max();
    T2 cell_upRight = std::numeric_limits<T2>::max();
    T2 cell_downLeft = 0;
    T2 cell_downRight = 0;
    
    for ( T2 n=0, nc=0; nc<=this->nCellx; ++nc ) {
        
        double x = this->xmin + nc*this->dx;
        
        for ( T2 nr=0; nr<=this->nCellz; ++nr ) {
            
            double z = this->zmin + nr*this->dz;
            
            if ( nr < this->nCellz && nc < this->nCellx ) {
                cell_downRight = nc*this->nCellz + nr;
            }
            else {
                cell_downRight = std::numeric_limits<T2>::max();
            }
            
            if ( nr > 0 && nc < this->nCellx ) {
                cell_upRight = nc*this->nCellz + nr - 1;
            }
            else {
                cell_upRight = std::numeric_limits<T2>::max();
            }
            
            if ( nr < this->nCellz && nc > 0 ) {
                cell_downLeft = (nc-1)*this->nCellz + nr;
            }
            else {
                cell_downLeft = std::numeric_limits<T2>::max();
            }
            
            if ( nr > 0 && nc > 0 ) {
                cell_upLeft = (nc-1)*this->nCellz + nr - 1;
            }
            else {
                cell_upLeft = std::numeric_limits<T2>::max();
            }
            
            // primary nodes
            //            std::cout << n << "\t p\t-\t" << x << '\t' << z
            //            << "\t-\t" << cell_upLeft
            //            << '\t' << cell_downLeft
            //            << '\t' << cell_upRight
            //            << '\t' << cell_downRight << '\n';
            
            if ( cell_upLeft != std::numeric_limits<T2>::max() ) {
                this->nodes[n].pushOwner( cell_upLeft );
            }
            if ( cell_downLeft != std::numeric_limits<T2>::max() ) {
                this->nodes[n].pushOwner( cell_downLeft );
            }
            if ( cell_upRight != std::numeric_limits<T2>::max() ) {
                this->nodes[n].pushOwner( cell_upRight );
            }
            if ( cell_downRight != std::numeric_limits<T2>::max() ) {
                this->nodes[n].pushOwner( cell_downRight );
            }
            
            this->nodes[n].setX( x );
            this->nodes[n].setZ( z );
            this->nodes[n].setGridIndex( n );
            this->nodes[n].setPrimary(5);
            
            ++n;
            
            // secondary nodes on the vertical
            if ( nr < this->nCellz ) {
                for (T2 ns=0; ns<nsnz; ++ns, ++n ) {
                    
                    double zsv = this->zmin + nr*this->dz + (ns+1)*dzs;
                    
                    //                    std::cout << n << "\tsv\t-\t" << x << '\t' << zsv << "\t-\t"
                    //                    << cell_downLeft << '\t' << cell_downRight << '\n';
                    
                    if ( cell_downLeft != std::numeric_limits<T2>::max() ) {
                        this->nodes[n].pushOwner( cell_downLeft );
                    }
                    if ( cell_downRight != std::numeric_limits<T2>::max() ) {
                        this->nodes[n].pushOwner( cell_downRight );
                    }
                    
                    this->nodes[n].setX( x );
                    this->nodes[n].setZ( zsv );
                    this->nodes[n].setGridIndex( n );
                }
            }
            
            // secondary nodes on the horizontal
            if ( nc < this->nCellx ) {
                for ( T2 ns=0; ns<nsnx; ++ns, ++n ) {
                    
                    double xsh = this->xmin + nc*this->dx + (ns+1)*dxs;
                    
                    //                    std::cout << n << "\tsh\t-\t" << xsh << '\t' << z << "\t-\t"
                    //                    << cell_upRight << '\t' << cell_downRight << '\n';
                    
                    if ( cell_upRight != std::numeric_limits<T2>::max() ) {
                        this->nodes[n].pushOwner( cell_upRight );
                    }
                    if ( cell_downRight != std::numeric_limits<T2>::max() ) {
                        this->nodes[n].pushOwner( cell_downRight );
                    }
                    
                    this->nodes[n].setX( xsh );
                    this->nodes[n].setZ( z );
                    this->nodes[n].setGridIndex( n );
                }
            }
            //            std::cout << '\n';
        }
        //        std::cout << '\n';
    }
}

template<typename T1, typename T2>
void Grid2Drisp<T1,T2>::interpSlownessSecondary() {  // TODO : test this
    
    T1 dxs = this->dx/(nsnx+1);
    T1 dzs = this->dz/(nsnz+1);
    
    for ( T2 n=0, nc=0; nc<=this->nCellx; ++nc ) {
        
        for ( T2 nr=0; nr<=this->nCellz; ++nr ) {
            
            T2 np1 = n;
            T2 np2v = np1 + nsnx+nsnz + 1;
            if ( nc == this->nCellx ) {
                np2v -= nsnx;
            }
            T2 np2h = np1 + (this->nCellz+1)*nsnx + this->nCellz*nsnz + this->nCellz+1;
            if ( nc == this->nCellx -1 ) {
                np2h -= nr*nsnx;
            }
            
            ++n;
            
            // secondary nodes on the vertical
            if ( nr < this->nCellz ) {
                T1 slope = (this->nodes[np2v].getNodeSlowness()-this->nodes[np1].getNodeSlowness())/this->dz;
                for (T2 ns=0; ns<nsnz; ++ns, ++n ) {
                    
                    T1 s = this->nodes[np1].getNodeSlowness() + slope * (ns+1)*dzs;
                    
                    this->nodes[n].setNodeSlowness( s );
                }
            }
            
            // secondary nodes on the horizontal
            if ( nc < this->nCellx ) {
                T1 slope = (this->nodes[np2h].getNodeSlowness()-this->nodes[np1].getNodeSlowness())/this->dx;
                for ( T2 ns=0; ns<nsnx; ++ns, ++n ) {
                    
                    T1 s = this->nodes[np1].getNodeSlowness() + slope * (ns+1)*dxs;
                    
                    this->nodes[n].setNodeSlowness( s );
                }
            }
        }
    }
}


template<typename T1, typename T2>
void Grid2Drisp<T1,T2>::initQueue(const std::vector<sxz<T1>>& Tx,
                                     const std::vector<T1>& t0,
                                     std::priority_queue<Node2Disp<T1,T2>*,
                                     std::vector<Node2Disp<T1,T2>*>,
                                     CompareNodePtr<T1>>& queue,
                                     std::vector<Node2Disp<T1,T2>>& txNodes,
                                     std::vector<bool>& inQueue,
                                     std::vector<bool>& frozen,
                                     const size_t threadNo) const {
    
    for (size_t n=0; n<Tx.size(); ++n) {
        bool found = false;
        for ( size_t nn=0; nn<this->nodes.size(); ++nn ) {
            if ( this->nodes[nn] == Tx[n] ) {
                found = true;
                this->nodes[nn].setTT( t0[n], threadNo );
                queue.push( &(this->nodes[nn]) );
                inQueue[nn] = true;
                frozen[nn] = true;
                break;
            }
        }
        if ( found==false ) {
            txNodes.push_back( Node2Disp<T1,T2>(t0[n], Tx[n], this->nThreads,
                                    threadNo) );
            // we belong to cell index no
            txNodes.back().pushOwner( this->getCellNo(Tx[n]) );
            txNodes.back().setGridIndex( static_cast<T2>(this->nodes.size()+
                                                         txNodes.size()-1) );
            
            queue.push( &(txNodes.back()) );
            inQueue.push_back( true );
            frozen.push_back( true );
        }
    }
}


template<typename T1, typename T2>
int Grid2Drisp<T1,T2>::raytrace(const std::vector<sxz<T1>>& Tx,
                                const std::vector<T1>& t0,
                                const std::vector<sxz<T1>>& Rx,
                                std::vector<T1>& traveltimes,
                                const size_t threadNo) const {
    
    if ( this->check_pts(Tx) == 1 ) return 1;
    if ( this->check_pts(Rx) == 1 ) return 1;
    
    for ( size_t n=0; n<this->nodes.size(); ++n ) {
        this->nodes[n].reinit( threadNo );
    }
    
    CompareNodePtr<T1> cmp(threadNo);
    std::priority_queue< Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
    CompareNodePtr<T1>> queue( cmp );
    
    std::vector<Node2Disp<T1,T2>> txNodes;
    std::vector<bool> inQueue( this->nodes.size(), false );
    std::vector<bool> frozen( this->nodes.size(), false );
    
    initQueue(Tx, t0, queue, txNodes, inQueue, frozen, threadNo);
    
    propagate(queue, inQueue, frozen, threadNo);
    
    if ( traveltimes.size() != Rx.size() ) {
        traveltimes.resize( Rx.size() );
    }
    
    for (size_t n=0; n<Rx.size(); ++n) {
        traveltimes[n] = this->getTraveltime(Rx[n], this->nodes, threadNo);
    }
    return 0;
}

template<typename T1, typename T2>
int Grid2Drisp<T1,T2>::raytrace(const std::vector<sxz<T1>>& Tx,
                                const std::vector<T1>& t0,
                                const std::vector<const std::vector<sxz<T1>>*>& Rx,
                                std::vector<std::vector<T1>*>& traveltimes,
                                const size_t threadNo) const {
    if ( this->check_pts(Tx) == 1 ) return 1;
    for ( size_t n=0; n<Rx.size(); ++n )
        if ( this->check_pts(*Rx[n]) == 1 ) return 1;
    
    for ( size_t n=0; n<this->nodes.size(); ++n ) {
        this->nodes[n].reinit( threadNo );
    }
    
    CompareNodePtr<T1> cmp(threadNo);
    std::priority_queue< Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
    CompareNodePtr<T1>> queue( cmp );
    
    std::vector<Node2Disp<T1,T2>> txNodes;
    std::vector<bool> inQueue( this->nodes.size(), false );
    std::vector<bool> frozen( this->nodes.size(), false );
    
    initQueue(Tx, t0, queue, txNodes, inQueue, frozen, threadNo);
    
    propagate(queue, inQueue, frozen, threadNo);
    
    if ( traveltimes.size() != Rx.size() ) {
        traveltimes.resize( Rx.size() );
    }
    
    for (size_t nr=0; nr<Rx.size(); ++nr) {
        traveltimes[nr]->resize( Rx[nr]->size() );
        for (size_t n=0; n<Rx[nr]->size(); ++n)
            (*traveltimes[nr])[n] = this->getTraveltime((*Rx[nr])[n], this->nodes, threadNo);
    }
    return 0;
    
}

template<typename T1, typename T2>
int Grid2Drisp<T1,T2>::raytrace(const std::vector<sxz<T1>>& Tx,
                                   const std::vector<T1>& t0,
                                   const std::vector<sxz<T1>>& Rx,
                                   std::vector<T1>& traveltimes,
                                   std::vector<std::vector<sxz<T1>>>& r_data,
                                   const size_t threadNo) const {
    
    if ( this->check_pts(Tx) == 1 ) return 1;
    if ( this->check_pts(Rx) == 1 ) return 1;
    
    for ( size_t n=0; n<this->nodes.size(); ++n ) {
        this->nodes[n].reinit( threadNo );
    }
    
    CompareNodePtr<T1> cmp(threadNo);
    std::priority_queue< Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
    CompareNodePtr<T1>> queue( cmp );
    std::vector<Node2Disp<T1,T2>> txNodes;
    std::vector<bool> inQueue( this->nodes.size(), false );
    std::vector<bool> frozen( this->nodes.size(), false );
    
    initQueue(Tx, t0, queue, txNodes, inQueue, frozen, threadNo);
    
    propagate(queue, inQueue, frozen, threadNo);
    
    if ( traveltimes.size() != Rx.size() ) {
        traveltimes.resize( Rx.size() );
    }
    if ( r_data.size() != Rx.size() ) {
        r_data.resize( Rx.size() );
    }
    for ( size_t ni=0; ni<r_data.size(); ++ni ) {
        r_data[ni].resize( 0 );
    }
    T2 nodeParentRx;
    T2 cellParentRx;
    
    for (size_t n=0; n<Rx.size(); ++n) {
        
        traveltimes[n] = this->getTraveltime(Rx[n], this->nodes, nodeParentRx, cellParentRx,
                                       threadNo);
        
        // Rx are in nodes (not txNodes)
        std::vector<Node2Disp<T1,T2>> *node_p;
        node_p = &(this->nodes);
        
        std::vector<sxz<double>> r_tmp;
        T2 iChild, iParent = nodeParentRx;
        sxz<double> child;
        
        // store the son's coord
        child.x = Rx[n].x;
        child.z = Rx[n].z;
        while ( (*node_p)[iParent].getNodeParent(threadNo) != std::numeric_limits<T2>::max() ) {
            
            r_tmp.push_back( child );
            
            // we now go up in time - parent becomes the child of grand'pa
            iChild = iParent;
            child.x = (*node_p)[iChild].getX();
            child.z = (*node_p)[iChild].getZ();
            
            // grand'pa is now papa
            iParent = (*node_p)[iChild].getNodeParent(threadNo);
            if ( iParent >= this->nodes.size() ) {
                node_p = &txNodes;
                iParent -= this->nodes.size();
            }
            else {
                node_p = &(this->nodes);
            }
        }
        
        // parent is now at Tx
        r_tmp.push_back( child );
        
        // finally, store Tx position
        child.x = (*node_p)[iParent].getX();
        child.z = (*node_p)[iParent].getZ();
        r_tmp.push_back( child );
        
        // the order should be from Tx to Rx, so we reorder...
        iParent = static_cast<T2>(r_tmp.size());
        r_data[n].resize( r_tmp.size() );
        for ( size_t nn=0; nn<r_data[n].size(); ++nn ) {
            r_data[n][nn].x = r_tmp[ iParent-1-nn ].x;
            r_data[n][nn].z = r_tmp[ iParent-1-nn ].z;
        }
    }
    return 0;
}

template<typename T1, typename T2>
int Grid2Drisp<T1,T2>::raytrace(const std::vector<sxz<T1>>& Tx,
                                   const std::vector<T1>& t0,
                                   const std::vector<const std::vector<sxz<T1>>*>& Rx,
                                   std::vector<std::vector<T1>*>& traveltimes,
                                   std::vector<std::vector<std::vector<sxz<T1>>>*>& r_data,
                                   const size_t threadNo) const {
    
    if ( this->check_pts(Tx) == 1 ) return 1;
    for ( size_t n=0; n<Rx.size(); ++n )
        if ( this->check_pts(*Rx[n]) == 1 ) return 1;
    
    for ( size_t n=0; n<this->nodes.size(); ++n ) {
        this->nodes[n].reinit( threadNo );
    }
    
    CompareNodePtr<T1> cmp(threadNo);
    std::priority_queue< Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
    CompareNodePtr<T1>> queue( cmp );
    
    std::vector<Node2Disp<T1,T2>> txNodes;
    std::vector<bool> inQueue( this->nodes.size(), false );
    std::vector<bool> frozen( this->nodes.size(), false );
    
    initQueue(Tx, t0, queue, txNodes, inQueue, frozen, threadNo);
    
    propagate(queue, inQueue, frozen, threadNo);
    
    if ( traveltimes.size() != Rx.size() ) {
        traveltimes.resize( Rx.size() );
    }
    if ( r_data.size() != Rx.size() ) {
        r_data.resize( Rx.size() );
    }
    
    for (size_t nr=0; nr<Rx.size(); ++nr) {
        
        traveltimes[nr]->resize( Rx[nr]->size() );
        r_data[nr]->resize( Rx[nr]->size() );
        for ( size_t ni=0; ni<r_data[nr]->size(); ++ni ) {
            (*r_data[nr])[ni].resize( 0 );
        }
        
        T2 nodeParentRx;
        T2 cellParentRx;
        
        for (size_t n=0; n<Rx[nr]->size(); ++n) {
            
            (*traveltimes[nr])[n] = this->getTraveltime((*Rx[nr])[n], this->nodes,
                                                  nodeParentRx, cellParentRx,
                                                  threadNo);
            
            bool flag=false;
            for ( size_t ns=0; ns<Tx.size(); ++ns ) {
                if ( (*Rx[nr])[n] == Tx[ns] ) {
                    
                    (*r_data[nr])[n].resize( 1 );
                    (*r_data[nr])[n][0].x = (*Rx[nr])[n].x;
                    (*r_data[nr])[n][0].z = (*Rx[nr])[n].z;
                    
                    flag = true;
                    break;
                }
            }
            if ( flag ) continue;
            
            // Rx are in nodes (not txNodes)
            std::vector<Node2Disp<T1,T2>> *node_p;
            node_p = &(this->nodes);
            
            std::vector<sxz<T1>> r_tmp;
            T2 iChild, iParent = nodeParentRx;
            sxz<T1> child;
            
            // store the son's coord
            child.x = (*Rx[nr])[n].x;
            child.z = (*Rx[nr])[n].z;
            while ( (*node_p)[iParent].getNodeParent(threadNo) !=
                   std::numeric_limits<T2>::max() ) {
                
                r_tmp.push_back( child );
                
                // we now go up in time - parent becomes the child of grand'pa
                iChild = iParent;
                child.x = (*node_p)[iChild].getX();
                child.z = (*node_p)[iChild].getZ();
                
                // grand'pa is now papa
                iParent = (*node_p)[iChild].getNodeParent(threadNo);
                if ( iParent >= this->nodes.size() ) {
                    node_p = &txNodes;
                    iParent -= this->nodes.size();
                }
                else {
                    node_p = &(this->nodes);
                }
            }
            
            // parent is now at Tx
            r_tmp.push_back( child );
            
            // finally, store Tx position
            child.x = (*node_p)[iParent].getX();
            child.z = (*node_p)[iParent].getZ();
            r_tmp.push_back( child );
            
            // the order should be from Tx to Rx, so we reorder...
            iParent = static_cast<T2>(r_tmp.size());
            (*r_data[nr])[n].resize( r_tmp.size() );
            for ( size_t nn=0; nn<(*r_data[nr])[n].size(); ++nn ) {
                (*r_data[nr])[n][nn].x = r_tmp[ iParent-1-nn ].x;
                (*r_data[nr])[n][nn].z = r_tmp[ iParent-1-nn ].z;
            }
            
        }
    }
    return 0;
    
}

template<typename T1, typename T2>
int Grid2Drisp<T1,T2>::raytrace(const std::vector<sxz<T1>>& Tx,
                                   const std::vector<T1>& t0,
                                   const std::vector<sxz<T1>>& Rx,
                                   std::vector<T1>& traveltimes,
                                   std::vector<std::vector<sxz<double>>>& r_data,
                                   std::vector<std::vector<siv<double>>>& l_data,
                                   const size_t threadNo) const {
    
    if ( check_pts(Tx) == 1 ) return 1;
    if ( check_pts(Rx) == 1 ) return 1;
    
    for ( size_t n=0; n<this->nodes.size(); ++n ) {
        this->nodes[n].reinit( threadNo );
    }
    
    CompareNodePtr<T1> cmp(threadNo);
    std::priority_queue< Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
    CompareNodePtr<T1>> queue( cmp );
    std::vector<Node2Disp<T1,T2>> txNodes;
    std::vector<bool> inQueue( this->nodes.size(), false );
    std::vector<bool> frozen( this->nodes.size(), false );
    
    initQueue(Tx, t0, queue, txNodes, inQueue, frozen, threadNo);
    
    propagate(queue, inQueue, frozen, threadNo);
    
    if ( traveltimes.size() != Rx.size() ) {
        traveltimes.resize( Rx.size() );
    }
    if ( l_data.size() != Rx.size() ) {
        l_data.resize( Rx.size() );
    }
    for ( size_t ni=0; ni<l_data.size(); ++ni ) {
        l_data[ni].resize( 0 );
    }
    if ( r_data.size() != Rx.size() ) {
        r_data.resize( Rx.size() );
    }
    for ( size_t ni=0; ni<r_data.size(); ++ni ) {
        r_data[ni].resize( 0 );
    }
    T2 nodeParentRx;
    T2 cellParentRx;
    
    for (size_t n=0; n<Rx.size(); ++n) {
        
        traveltimes[n] = getTraveltime(Rx[n], this->nodes, nodeParentRx, cellParentRx,
                                       threadNo);
        
        // Rx are in nodes (not txNodes)
        std::vector<Node2Disp<T1,T2>> *node_p;
        node_p = &(this->nodes);
        
        std::vector<sxz<double>> r_tmp;
        T2 iChild, iParent = nodeParentRx;
        sxz<double> child;
        siv<double> cell;
        
        // store the son's coord
        child.x = Rx[n].x;
        child.z = Rx[n].z;
        cell.i = cellParentRx;
        while ( (*node_p)[iParent].getNodeParent(threadNo) != std::numeric_limits<T2>::max() ) {
            
            r_tmp.push_back( child );
            
            cell.v = (*node_p)[iParent].getDistance( child );
            bool found=false;
            for (size_t nc=0; nc<l_data[n].size(); ++nc) {
                if ( l_data[n][nc].i == cell.i ) {
                    l_data[n][nc].v += cell.v;
                    found = true;
                }
            }
            if ( found == false ) {
                l_data[n].push_back( cell );
            }
            
            // we now go up in time - parent becomes the child of grand'pa
            iChild = iParent;
            child.x = (*node_p)[iChild].getX();
            child.z = (*node_p)[iChild].getZ();
            cell.i = (*node_p)[iChild].getCellParent(threadNo);
            
            // grand'pa is now papa
            iParent = (*node_p)[iChild].getNodeParent(threadNo);
            if ( iParent >= this->nodes.size() ) {
                node_p = &txNodes;
                iParent -= this->nodes.size();
            }
            else {
                node_p = &(this->nodes);
            }
        }
        
        // parent is now at Tx
        r_tmp.push_back( child );
        
        cell.v = (*node_p)[iParent].getDistance( child );
        bool found=false;
        for (size_t nc=0; nc<l_data[n].size(); ++nc) {
            if ( l_data[n][nc].i == cell.i ) {
                l_data[n][nc].v += cell.v;
                found = true;
            }
        }
        if ( found == false ) {
            l_data[n].push_back( cell );
        }
        
        // finally, store Tx position
        child.x = (*node_p)[iParent].getX();
        child.z = (*node_p)[iParent].getZ();
        r_tmp.push_back( child );
        
        //  must be sorted to build matrix L
        sort(l_data[n].begin(), l_data[n].end(), CompareSiv_i<T1>());
        
        // the order should be from Tx to Rx, so we reorder...
        iParent = r_tmp.size();
        r_data[n].resize( r_tmp.size() );
        for ( size_t nn=0; nn<r_data[n].size(); ++nn ) {
            r_data[n][nn].x = r_tmp[ iParent-1-nn ].x;
            r_data[n][nn].z = r_tmp[ iParent-1-nn ].z;
        }
    }
    return 0;
}


template<typename T1, typename T2>
void Grid2Drisp<T1,T2>::propagate( std::priority_queue<Node2Disp<T1,T2>*, std::vector<Node2Disp<T1,T2>*>,
                                     CompareNodePtr<T1>>& queue,
                                     std::vector<bool>& inQueue,
                                     std::vector<bool>& frozen,
                                     const size_t threadNo) const {
    
    while ( !queue.empty() ) {
        const Node2Disp<T1,T2>* source = queue.top();
        queue.pop();
        inQueue[ source->getGridIndex() ] = false;
        
        for ( size_t no=0; no<source->getOwners().size(); ++no ) {
            
            T2 cellNo = source->getOwners()[no];
            
            for ( size_t k=0; k< this->neighbors[cellNo].size(); ++k ) {
                T2 neibNo = this->neighbors[cellNo][k];
                if ( neibNo == source->getGridIndex() || frozen[neibNo] ) {
                    continue;
                }
                
                // compute dt
                T1 dt = this->computeDt(*source, this->nodes[neibNo]);
                
                if ( source->getTT(threadNo)+dt < this->nodes[neibNo].getTT(threadNo) ) {
                    this->nodes[neibNo].setTT( source->getTT(threadNo)+dt, threadNo );
                    this->nodes[neibNo].setnodeParent( source->getGridIndex(), threadNo );
                    this->nodes[neibNo].setCellParent( cellNo, threadNo );
                    
                    if ( !inQueue[neibNo] ) {
                        queue.push( &(this->nodes[neibNo]) );
                        inQueue[neibNo] = true;
                    }
                }
            }
        }
    }
}





#endif /* Grid2Drisp_h */