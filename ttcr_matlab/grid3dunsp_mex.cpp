//
//  grid3duisp_mex.cpp
//  ttcr
//
//  Created by Bernard Giroux on 2015-03-11.
//  Copyright (c) 2015 Bernard Giroux. All rights reserved.
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

#include <exception>
#include <thread>

#include "mex.h"
#include "class_handle.hpp"

#include "Grid3Dunsp.h"

using namespace std;
using namespace ttcr;

typedef Grid3Dunsp<double,uint32_t> grid;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Get the command string
    char cmd[64];
    if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
        mexErrMsgTxt("First input should be a command string less than 64 characters long.");
    
    //  ---------------------------------------------------------------------------
    // New
    if (!strcmp("new", cmd)) {
        // Check parameters
        if (nlhs != 1) {
            mexErrMsgTxt("New: One output expected.");
        }
        if (nrhs != 3 && nrhs != 4) {
            mexErrMsgTxt("New: 2 or 3 input arguments needed.");
        }
        // Return a handle to a new C++ instance
        
        //
        // Nodes
        //
        if (!(mxIsDouble(prhs[1]))) {
            mexErrMsgTxt("Node coordiates must be double precision.");
        }
        double *xyz = static_cast<double*>( mxGetPr(prhs[1]) );
        mwSize number_of_dims = mxGetNumberOfDimensions(prhs[1]);
        if ( number_of_dims != 2 ) {
            mexErrMsgTxt("Node coordiates must be a matrix (nNodes by 3).");
        }
        const mwSize *dim_array = mxGetDimensions(prhs[1]);
        size_t nXYZ = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 3 ) {
            mexErrMsgTxt("Node coordiates must be a matrix (nNodes by 3).");
        }
        vector<sxyz<double>> nodes(nXYZ);
        for ( size_t n=0; n<nXYZ; ++n ) {
            nodes[n].x = xyz[n];
            nodes[n].y = xyz[n+nXYZ];
            nodes[n].z = xyz[n+2*nXYZ];
        }
        
        //
        // Tetrahedra
        //
        if (!(mxIsDouble(prhs[2]))) {
            mexErrMsgTxt("Tetrahedra node indices must be double precision.");
        }
        double *ind = static_cast<double*>( mxGetPr(prhs[2]) );
        number_of_dims = mxGetNumberOfDimensions(prhs[2]);
        if ( number_of_dims != 2 ) {
            mexErrMsgTxt("Tetrahedra node indices coordiates must be a matrix (nTetrahedra by 4).");
        }
        dim_array = mxGetDimensions(prhs[2]);
        size_t nTet = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 4 ) {
            mexErrMsgTxt("Tetrahedra node indices coordiates must be a matrix (nTetrahedra by 4).");
        }
        vector<tetrahedronElem<uint32_t>> tetrahedra(nTet);
        for ( size_t n=0; n<nTet; ++n ) {
            tetrahedra[n].i[0] = static_cast<uint32_t>( ind[n]-1 );
            tetrahedra[n].i[1] = static_cast<uint32_t>( ind[n+nTet]-1 );
            tetrahedra[n].i[2] = static_cast<uint32_t>( ind[n+2*nTet]-1 );
            tetrahedra[n].i[3] = static_cast<uint32_t>( ind[n+3*nTet]-1 );
        }
        
        //
        // Number of secondary nodes
        //
        uint32_t nSecondary = 5;
        if ( nrhs == 4 ) {
            if (!(mxIsDouble(prhs[3]))) {
                mexErrMsgTxt("Number of secondary nodes must be double precision.");
            }
            if ( mxGetNumberOfElements(prhs[3]) != 1 ) {
                mexErrMsgTxt("Number of secondary nodes must be a scalar.");
            }
            nSecondary = static_cast<uint32_t>( mxGetScalar(prhs[3]) );
        }
        
        plhs[0] = convertPtr2Mat<grid>(new grid(nodes, tetrahedra, nSecondary));
        return;
    }
    
    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
        mexErrMsgTxt("Second input should be a class instance handle.");
    
    // ---------------------------------------------------------------------------
    // Delete
    //
    if (!strcmp("delete", cmd)) {
        // Destroy the C++ object
        destroyObject<grid>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return;
    }
    
    // Get the class instance pointer from the second input
    grid *grid_instance = convertMat2Ptr<grid>(prhs[1]);
    
    
    
    // Call the various class methods
    // ---------------------------------------------------------------------------
    // setSlowness
    //
    if (!strcmp("setSlowness", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs != 3)
            mexErrMsgTxt("setSlowness: Unexpected arguments.");
        // Call the method
        
        if (!(mxIsDouble(prhs[2]))) {
            mexErrMsgTxt("Slowness must be double precision.");
        }
        double *slowness = static_cast<double*>( mxGetPr(prhs[2]) );
        mwSize number_of_dims = mxGetNumberOfDimensions(prhs[2]);
        if ( number_of_dims != 2 ) {
            mexErrMsgTxt("Slowness must be a vector (nSlowness by 1).");
        }
        const mwSize *dim_array = mxGetDimensions(prhs[2]);
        size_t nSlowness = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 1 ) {
            mexErrMsgTxt("Slowness must be a vector (nSlowness by 1).");
        }
        
        try {
            grid_instance->setSlowness(slowness, nSlowness);
        } catch (std::exception& e) {
            mexErrMsgTxt("Slowness values must be defined for each grid node.");
        }
        
        return;
    }
    
    //  ---------------------------------------------------------------------------
    // raytrace
    if (!strcmp("raytrace", cmd)) {
        // Check parameters
        
        if ( nrhs != 5 && nrhs != 6 ) {
            mexErrMsgTxt("raytrace: Unexpected arguments.");
        }
        if (nlhs > 2) {
            mexErrMsgTxt("raytrace has a maximum of two output argument.");
        }
        //
        // Slowness
        //
        if (!(mxIsDouble(prhs[2]))) {
            mexErrMsgTxt("Slowness must be double precision.");
        }
        double *slowness = static_cast<double*>( mxGetPr(prhs[2]) );
        mwSize number_of_dims = mxGetNumberOfDimensions(prhs[2]);
        if ( number_of_dims != 2 ) {
            mexErrMsgTxt("Slowness must be a vector (nSlowness by 1).");
        }
        const mwSize *dim_array = mxGetDimensions(prhs[2]);
        size_t nSlowness = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 1 ) {
            mexErrMsgTxt("Slowness must be a vector (nSlowness by 1).");
        }
        
        try {
            grid_instance->setSlowness(slowness, nSlowness);
        } catch (std::exception& e) {
            mexErrMsgTxt("Slowness values must be defined for each grid node.");
        }
        
        //
        // Tx
        //
        if (!(mxIsDouble(prhs[3]))) {
            mexErrMsgTxt("Tx must be double precision.");
        }
        number_of_dims = mxGetNumberOfDimensions(prhs[3]);
        if ( number_of_dims != 2 ){
            mexErrMsgTxt("Tx must be a rank 2 matrix.");
        }
        dim_array = mxGetDimensions(prhs[3]);
        size_t nTx = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 3 ) {
            mexErrMsgTxt("Tx: matrix nTx by 3.");
        }
        double *Tx = static_cast<double*>( mxGetPr(prhs[3]) );
        
        //
        // Rx
        //
        if (!(mxIsDouble(prhs[4]))) {
            mexErrMsgTxt("Rx must be double precision.");
        }
        number_of_dims = mxGetNumberOfDimensions(prhs[4]);
        if ( number_of_dims != 2 ){
            mexErrMsgTxt("Rx must be a rank 2 matrix.");
        }
        dim_array = mxGetDimensions(prhs[4]);
        size_t nRx = static_cast<size_t>( dim_array[0] );
        if ( dim_array[1] != 3 ) {
            mexErrMsgTxt("Rx: matrix nRx by 3.");
        }
        double *Rx = static_cast<double*>( mxGetPr(prhs[4]) );
        
        if ( nTx != nRx ) {
            mexErrMsgTxt("nTx should be equal to nRx.");
        }
        
        //
        // t0
        //
        double *tTx;
        if ( nrhs == 6 ) {
            if (!(mxIsDouble(prhs[5]))) {
                mexErrMsgTxt("t0 must be double precision.");
            }
            number_of_dims = mxGetNumberOfDimensions(prhs[5]);
            if ( number_of_dims != 2 ){
                mexErrMsgTxt("t0 must be a rank 2 matrix.");
            }
            dim_array = mxGetDimensions(prhs[5]);
            size_t nT0 = static_cast<size_t>( dim_array[0] );
            if ( dim_array[1] != 1 || nT0 != nTx ) {
                mexErrMsgTxt("t0: matrix nTx by 1.");
            }
            tTx = static_cast<double*>( mxGetPr(prhs[5]) );
        } else {
            tTx = new double [nTx];
            for ( size_t n=0; n<nTx; ++n ) tTx[n] = 0.0;
        }
        
        /* ------------------------------------------------------
         Output variable
         ------------------------------------------------------ */
        
        plhs[0] = mxCreateDoubleMatrix(nRx, 1, mxREAL);
        double *t_arr = mxGetPr(plhs[0]);
        
        
        /* ------------------------------------------------------
         Optional output variables
         ------------------------------------------------------ */
        
        mxArray **Rays;
        if ( nlhs == 2 ) {
            // 2rd arg: rays.
            plhs[1] = mxCreateCellMatrix(nRx, 1);
            Rays = (mxArray **) mxCalloc(nRx, sizeof(mxArray *));
        }
        
        /*
         Looking for redundants Tx pts
         */
        vector<vector<sxyz<double>>> vTx;
        vector<vector<double>> t0;
        vector<vector<size_t>> iTx;
        sxyz<double> sxyz_tmp;
        sxyz_tmp.x = Tx[0];
        sxyz_tmp.y = Tx[nTx];
        sxyz_tmp.z = Tx[2*nTx];
        vTx.push_back( vector<sxyz<double> >(1, sxyz_tmp) );
        t0.push_back( vector<double>(1, tTx[0]) );
        iTx.push_back( vector<size_t>(1, 0) );  // indices of Rx corresponding to current Tx
        for ( size_t ntx=1; ntx<nTx; ++ntx ) {
            sxyz_tmp.x = Tx[ntx];
            sxyz_tmp.y = Tx[ntx+nTx];
            sxyz_tmp.z = Tx[ntx+2*nTx];
            bool found = false;
            
            for ( size_t nv=0; nv<vTx.size(); ++nv ) {
                if ( vTx[nv][0].x==sxyz_tmp.x &&
                    vTx[nv][0].y==sxyz_tmp.y &&
                    vTx[nv][0].z==sxyz_tmp.z ) {
                    found = true;
                    iTx[nv].push_back( ntx ) ;
                    break;
                }
            }
            if ( !found ) {
                vTx.push_back( vector<sxyz<double>>(1, sxyz_tmp) );
                t0.push_back( vector<double>(1, tTx[ntx]) );
                iTx.push_back( vector<size_t>(1, ntx) );
            }
        }
        
        if ( nrhs == 5 ) {
            delete [] tTx;
        }
        
        
        /*
         Looping over all non redundant Tx
         */
        
        vector<sxyz<double>> vRx;
        vector<vector<double>> tt( vTx.size() );
        vector<vector<vector<sxyz<double>>>> r_data( vTx.size() );
        
        if ( grid_instance->getNthreads() == 1 || vTx.size()<=grid_instance->getNthreads() ) {
            for ( size_t nv=0; nv<vTx.size(); ++nv ) {
                
                vRx.resize( 0 );
                for ( size_t ni=0; ni<iTx[nv].size(); ++ni ) {
                    sxyz_tmp.x = Rx[ iTx[nv][ni] ];
                    sxyz_tmp.y = Rx[ iTx[nv][ni]+nRx ];
                    sxyz_tmp.z = Rx[ iTx[nv][ni]+2*nRx ];
                    vRx.push_back( sxyz_tmp );
                }
                
                if ( nlhs == 2 ) {
                    try {
                        grid_instance->raytrace(vTx[nv], t0[nv], vRx, tt[nv], r_data[nv]);
                    } catch (...) {
                        mexErrMsgTxt("Problem while raytracing.");
                    }
                }
                else {
                    try {
                        grid_instance->raytrace(vTx[nv], t0[nv], vRx, tt[nv]);
                    } catch (...) {
                        mexErrMsgTxt("Problem while raytracing.");
                    }
                }
            }
        } else {
            size_t num_threads = grid_instance->getNthreads() < vTx.size() ? grid_instance->getNthreads() : vTx.size();
            vector<size_t> blk_size(num_threads, 0);
            size_t nj = vTx.size();
            while (nj > 0) {
                for ( size_t i=0; i<num_threads; ++i ) {
                    blk_size[i] += 1;
                    nj -= 1;
                    if ( nj == 0 ) {
                        break;
                    }
                }
            }
            
            vector<thread> threads(num_threads);
            size_t blk_start = 0;
            for ( size_t i=0; i<num_threads; ++i ) {
                
                size_t blk_end = blk_start + blk_size[i];
                
                threads[i]=thread( [&grid_instance,&vTx,&tt,&t0,&Rx,&iTx,&nRx,
                                    &nlhs,&r_data,blk_start,blk_end,i]{
                    
                    for ( size_t nv=blk_start; nv<blk_end; ++nv ) {
                        
                        sxyz<double> sxyz_tmp;
                        vector<sxyz<double>> vRx;
                        for ( size_t ni=0; ni<iTx[nv].size(); ++ni ) {
                            sxyz_tmp.x = Rx[ iTx[nv][ni] ];
                            sxyz_tmp.y = Rx[ iTx[nv][ni]+nRx ];
                            sxyz_tmp.z = Rx[ iTx[nv][ni]+2*nRx ];
                            vRx.push_back( sxyz_tmp );
                        }
                        if ( nlhs == 2 ) {
                            try {
                                grid_instance->raytrace(vTx[nv], t0[nv], vRx, tt[nv], r_data[nv], i);
                            } catch (...) {
                                mexErrMsgTxt("Problem while raytracing.");
                            }
                        } else {
                            try {
                                grid_instance->raytrace(vTx[nv], t0[nv], vRx, tt[nv], i);
                            } catch (...) {
                                mexErrMsgTxt("Problem while raytracing.");
                            }
                        }
                    }
                });
                
                blk_start = blk_end;
            }
            
            std::for_each(threads.begin(),threads.end(),
                          std::mem_fn(&std::thread::join));
        }
        
        for ( size_t nv=0; nv<vTx.size(); ++nv ) {
            for ( size_t ni=0; ni<iTx[nv].size(); ++ni ) {
                t_arr[ iTx[nv][ni] ] = tt[nv][ni];
            }
        }
        
        if ( nlhs == 2 ) {
            for ( size_t nv=0; nv<vTx.size(); ++nv ) {
                for ( size_t ni=0; ni<iTx[nv].size(); ++ni ) {
                    size_t npts = r_data[nv][ni].size();
                    Rays[ iTx[nv][ni] ] = mxCreateDoubleMatrix(npts, 3, mxREAL);
                    double *rays_p = (double*) mxGetData(Rays[ iTx[nv][ni] ]);
                    for ( size_t np=0; np<npts; ++np ) {
                        rays_p[np] = r_data[nv][ni][np].x;
                        rays_p[np+npts] = r_data[nv][ni][np].y;
                        rays_p[np+2*npts] = r_data[nv][ni][np].z;
                    }
                    mxSetCell( plhs[1], iTx[nv][ni], Rays[ iTx[nv][ni] ] );
                }
            }
        }
        
        return;
    }
    
    // Got here, so command not recognized
    mexErrMsgTxt("Command not recognized.");
}
