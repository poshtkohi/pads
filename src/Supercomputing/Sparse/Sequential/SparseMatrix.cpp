/**
    #define meta ...
    printf("%s\n", meta);
**/

/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "SparseMatrix.h"

//**************************************************************************************************************//

namespace Supercomputing::Sparse::Sequential
{
    //----------------------------------------------------
    //template<typename T> SparseMatrix<T>::SparseMatrix(unsigned int m, unsigned int n)
    SparseMatrix::SparseMatrix(unsigned int m, unsigned int n, unsigned int nzmax)
    {
        if(m == 0)
            throw range_error("m must be a positive integer!");

        if(n == 0)
            throw range_error("n must be a positive integer!");

        if(nzmax > m * n)
            throw range_error("nzmax cannot be greater than matrix dimensions!");

        _m = m;
        _n = n;

        _p = new std::vector<int>(_n + 1, 0);
        _i = new std::vector<int>(nzmax);
        _x = new std::vector<double>(nzmax);

        _nz = 0;
        _nzmax = nzmax;

        //for(std::vector<unsigned int>::iterator it = _p->begin() ; it != _p->end() ; ++it)
        //{
        //    unsigned int val = *it;
        //    std::cout << val << std::endl;
        //}
    }
    //----------------------------------------------------
    SparseMatrix::SparseMatrix(unsigned int m, unsigned int n) : SparseMatrix(m, n, 0)
    {
    }
    //----------------------------------------------------
    SparseMatrix::SparseMatrix(int m, int n, const std::vector<int> &p, const std::vector<int> &i, const std::vector<double> &x)
    {
        if(m < 0)
            throw std::range_error("m must be a positive number!");

        if(n < 0)
            throw std::range_error("n must be a positive number!");

        if(p.size() == 0 || i.size() == 0 || x.size() == 0)
            throw std::range_error("Size of p, i or i must be greater than zero!");

        if(p.size() != n + 1)
        {
            printf("p.size=%d n+1=%d\n", p.size(), n + 1);
            throw std::range_error("Size of p must be n + 1");
        }


        _m = m;
        _n = n;

        _p = new std::vector<int>(p);
        _i = new std::vector<int>(i);
        _x = new std::vector<double>(x);

        _nz = i.size();
        _nzmax = i.size();
    }
    //----------------------------------------------------
    SparseMatrix::~SparseMatrix()
    {
        delete _p;
        delete _i;
        delete _x;
    }
    //----------------------------------------------------
    void SparseMatrix::Set(int row, int column, double val)
    {
        Validate(row, column);

        /*if(_i->size() == 0)
        {
            //std::cout << "Insert 1" << std::endl;
            Insert(0, row, column, val);
            //std::cout << "Insert " << std::endl;
            return;
        }

        if(val == 0 && (*_p)[column+1] == 0)
            return;*/

        int pos = 0;
        int r1 = (*_p)[column] ;
        int r2 = (*_p)[column + 1] - 1;

        //printf("r1=%d r2=%d\n", r1, r2);

        if(r2 < r1 || r2 < 0)
        {
            if(val == 0)
                return;

            pos = r1;
            Insert(pos, row, column, val);
            return;
            //goto Here;
        }

        if(r1 == r2)
        {
            if(val == 0)
                return;

            pos = r1 + 1;
            Insert(pos, row, column, val);
            return;
            //goto Here;
        }

        // First look for an existing value at (row, column)
        bool found = false;
        for(int r = r1 ; r <= r2 ; ++r)
        {
            if( (*_i)[r] == row )
            {
                pos = r;
                found = true;
                break;
            }
            else if( (*_i)[r] < row )
            {
                // To insert after r
                pos = r + 1;
            }
        }

        if(found)
        {
            // The element has been marked for deletion

            if(val == 0)
            {
                //std::cout << "remove pos " << pos << "c olumn " << column<< std::endl;
                Remove(pos, column);
            }
            else
                (*_x)[pos] = val;
            //std::cout << "update " << std::endl;
        }
        else
        {
            //std::cout << "Insert 3" << std::endl;
            Insert(pos, row, column, val);
        }
    }
    //----------------------------------------------------
    double SparseMatrix::Get(int row, int column)
    {
        Validate(row, column);

        /*if(_i->size() == 0 || (*_p)[column + 1] == 0)
        {
            //printf("p1\n");
            return 0;
        }*/

        //unsigned int from = (*_i)[ (*_p)[column] ];
        //unsigned int to = (*_i)[ (*_p)[column + 1] - 1 ];

        //std::cout << "get from " << from << " to " << to << std::endl;

        /*for(std::vector<double>::iterator it = _x->begin() ; it != _x->end() ; ++it)
        {
            double x = *it;
            std::cout << x << std::endl;
        }*/

        int r1 = (*_p)[column] ;
        int r2 = (*_p)[column + 1] - 1;

        if(r2 < r1 || r2 < 0)
        {
            //printf("here\n");
            return 0;
        }

        bool found = false;
        int pos = 0;
        for(int r = r1 ; r <= r2 ; ++r)
        {
            //std::cout << "r " << r << " (*_i)[r] " << (*_i)[r] << std::endl;

            if( (*_i)[r] == row )
            {
                pos = r;
                found = true;
                break;
            }
        }

        if(found)
        {
            //printf("p3\n");
            return (*_x)[pos];
        }
        else
        {
            //printf("p4\n");
            return 0;
        }
    }
    //----------------------------------------------------
    SparseMatrix *SparseMatrix::Add(const SparseMatrix *A, const SparseMatrix *B)
    {
        if(A->_n != B->_n || A->_m != B->_m)
            throw std::range_error("The numbers of rows and columns of the two matrices must be equal!");

        SparseMatrix *C = new SparseMatrix(A->_m, A->_n, A->_nz + B->_nz);
        //SparseMatrix *C = new SparseMatrix(A->_n, A->_m);

        //printf("Addd\n");
        //return C;

        // Allocate matrix C such that it is equal to A.
        /*int num_of_rows = A->_m;
        int num_of_columns = A->_n;
        C->_i->resize(A->_nz);
        C->_x->resize(A->_nz);
        memcpy(&(*C->_p)[0], &(*A->_p)[0], (1 + num_of_columns) * sizeof(int));
        memcpy(&(*C->_i)[0], &(*A->_i)[0], A->_nz * sizeof(int));
        memcpy(&(*C->_x)[0], &(*A->_x)[0], A->_nz * sizeof(double));*/
        //Copy(A, C);
        int num_of_rows = A->_m;
        int num_of_columns = A->_n;
        std::vector<int> w(num_of_rows);     // workspace
        std::vector<double> x(num_of_rows);  // dense vector workspace
        int nz_total = 0;
        int start_index_of_Ci = 0;

        for(int j = 0 ; j < num_of_columns ; ++j)
        {
            int nz = 0;
            // Copy A*j into C*J
            //Allocate matrix C
            // Traverse A
            //printf("j=%d", j);
            nz = SplashColumn(A, C, j, nz, start_index_of_Ci, addition, w, x);
            nz = SplashColumn(B, C, j, nz, start_index_of_Ci, addition, w, x);
            // Copy A*j into C*J
            // Now, copy x into Cx.
            for(int i = start_index_of_Ci, k = 1 ; k <= nz ; ++i, ++k)
            {
                int iIndex = (*C->_i)[i];
                double val = x[iIndex];
                //C->_i->push_back(iIndex);
                //C->_x->push_back(val);
                (*C->_i)[i] = iIndex;
                (*C->_x)[i] = val;
                //printf("here2 i=%d iIndex=%d val=%g\n", i, iIndex, val);
            }
            nz_total += nz;
            (*C->_p)[j + 1] = nz_total;
            // Now, copy x into Cx.
            start_index_of_Ci += nz;
            //break;
        }

        //std::cout << "size " << C->_i->size() << std::endl;
        /*std::cout << "nz_total " << nz_total << std::endl;
        std::cout << "x";
        for(int i = 0 ; i < 4; ++i)
        {
            //std::cout << " " << x[i];
            //std::cout << " " << (*C->_p)[i];
            std::cout << " " << (*C->_i)[i];
        }
        std::cout << std::endl;*/

        for(int i = 0 ; i < nz_total ; ++i)
        {
            std::cout << " " << (*C->_x)[i];
        }
        std::cout << std::endl;
        // Cleanup C
        C->_i->resize(nz_total);
        C->_x->resize(nz_total);
        C->_nz = nz_total;
        return C;
    }
    //----------------------------------------------------
    SparseMatrix *SparseMatrix::Multiply(const SparseMatrix *A, const SparseMatrix *B)
    {
        if(A->_n != B->_m)
            throw std::range_error("The number of columns of the 1st matrix must equal the number of rows of the 2nd matrix!");

        SparseMatrix *C = new SparseMatrix(A->_m, B->_n, A->_nz + B->_nz);

        int num_of_rows = C->_m;
        int num_of_columns = C->_n;
        std::vector<int> w(num_of_rows);     // workspace
        std::vector<double> x(num_of_rows);  // dense vector workspace
        int nz_total = 0;

        for(int jC = 0 ; jC < num_of_columns ; ++jC)
        {
            int nz = 0;

            // Iterates over B*j
            int i1B = (*B->_p)[jC];         // Start index in _i
            int i2B = (*B->_p)[jC + 1] - 1; // End index in _i

            //printf("jC=%d i1B=%d i2B=%d\n", jC, i1B, i2B);

            for(int iB = i1B ; iB <= i2B ; ++iB)
            {
                int iIndexB = (*B->_i)[iB];
                double valB = (*B->_x)[iB];

                // Iterates over A*j
                int i1A = (*A->_p)[iIndexB];            // Start index in _i
                int i2A = (*A->_p)[iIndexB + 1] - 1;    // End index in _i

                for(int iA = i1A ; iA <= i2A ; ++iA)
                {
                    int iIndexA = (*A->_i)[iA];
                    double valA = (*A->_x)[iA];

                    if(w[iIndexA] == jC + 1) // it means that the item exists in w
                    {
                        x[iIndexA] += valA * valB;
                    }
                    else
                    {
                        // Allocate space
                        if(C->_nzmax < nz_total + 1)
                        {
                            ///printf("nzmax=%d nz_total=%d\n", C->_nzmax, nz_total + 1);
                            C->_nzmax = (nz_total + 1) * 2;
                            C->_i->resize(C->_nzmax);
                            C->_x->resize(C->_nzmax);
                        }
                        w[iIndexA] = jC + 1;
                        //C->_i->push_back(iIndex);
                        (*C->_i)[nz_total] = iIndexA;
                        //_x->push_back(val);
                        x[iIndexA] = valA * valB;
                        nz++;
                        nz_total++;
                    }
                }
            }

            for(int i = nz_total - nz, k = 1 ; k <= nz ; ++i, ++k)
            {
                int iIndex = (*C->_i)[i];
                double val = x[iIndex];
                //C->_i->push_back(iIndex);
                //C->_x->push_back(val);
                (*C->_i)[i] = iIndex;
                (*C->_x)[i] = val;
                //printf("here2 i=%d iIndex=%d val=%g\n", i, iIndex, val);
            }
            //nz_total += nz;
            (*C->_p)[jC + 1] = nz_total;
            //break;
        }

        // Cleanup C
        C->_i->resize(nz_total);
        C->_x->resize(nz_total);
        C->_nz = nz_total;
        C->_nzmax = nz_total;

        return C;
    }
    //----------------------------------------------------
    void SparseMatrix::MultiplyByVector(const SparseMatrix *A, const std::vector<double> &v, std::vector<double> &r)
    {
        //if(A->_m != A->_n)
        //    throw std::range_error("The number of rows and columns of A matrix must be equal!");

        if(A->_n != v.size())
            throw std::range_error("Size of v must be equal to the number of columns of A matrix!");

        r.resize(v.size());
        memset(&r[0], 0, r.size() * sizeof(double));

        for(int j = 0 ; j < A->_n ; ++j)
        {
            // Iterates over A*j
            int i1 = (*A->_p)[j];         // Start index in _i
            int i2 = (*A->_p)[j + 1] - 1; // End index in _i

            //printf("jC=%d i1B=%d i2B=%d\n", jC, i1B, i2B);

            for(int i = i1 ; i <= i2 ; ++i)
            {
                int iIndex = (*A->_i)[i];
                double val = (*A->_x)[i];
                r[iIndex] += val * v[j];
            }
        }
    }
    //----------------------------------------------------
    /*SparseMatrix *SparseMatrix::Multiply(const SparseMatrix *A, const SparseMatrix *B)
    {
        // Number of rows
        //public: int _m;
        // Number of columns
        //public: int _n;
        if(A->_n != B->_m)
            throw std::range_error("The number of columns of the 1st matrix must equal the number of rows of the 2nd matrix!");

        // The result will have the same number of rows as the 1st matrix, and the same number of columns as the 2nd matrix.
        SparseMatrix *C = new SparseMatrix(A->_m, B->_n, A->_nz + B->_nz);
         printf("xxxxx: %d \n", A->_nz + B->_nz);
        //return C;
        //SparseMatrix *C = new SparseMatrix(A->_n, A->_m);

        //printf("Addd\n");
        //return C;

        int num_of_rows = C->_m;
        int num_of_columns = C->_n;
        std::vector<int> w(num_of_rows);     // workspace
        std::vector<double> x(num_of_rows);  // dense vector workspace
        std::vector<int> ww(num_of_rows);     // workspace
        std::vector<double> xx(num_of_rows);  // dense vector workspace
        int nz_total = 0;
        int start_index_of_Ci = 0;

        for(int jC = 0 ; jC < num_of_columns ; ++jC)
        {
            printf("jC=%d\n", jC);
            int nz = 0;
            // Iterates over A
            for(int jA = 0 ; jA < A->_n ; ++jA)
            {
                //if(i2A < i1A || i2A < 0)
                //    continue;

                // Iterates over A*j
                int i1A = (*A->_p)[jA];         // Start index in _i
                int i2A = (*A->_p)[jA + 1] - 1; // End index in _i

                for(int iA = i1A ; iA <= i2A ; ++iA)
                {
                    int iIndexA = (*A->_i)[iA];
                    double valA = (*A->_x)[iA];

                    w[iIndexA] = jC + jA + 1;
                    x[iIndexA] = valA;
                }
                // Iterates over B*j
                int i1B = (*B->_p)[jC];         // Start index in _i
                int i2B = (*B->_p)[jC + 1] - 1; // End index in _i

                for(int iB = i1B ; iB <= i2B ; ++iB)
                {
                    int iIndexB = (*B->_i)[iB];
                    double valB = (*B->_x)[iB];
                    // Corresponding elements were found where column number of A is equal to row number of B
                    if(w[iIndexB] == jC + jA + 1)
                    {
                        x[iIndexB] *= valB;
                        // Insert into C or add it if it exists
                        if(ww[iIndexB] == jC + jA + 1)
                        {
                            xx[iIndexB] += x[iIndexB];
                            //ww
                            //std::vector<double> xx(num_of_rows);  // dense vector workspace
                        }
                        else
                        {
                            // Allocate space
                            if(C->_nzmax < nz_total + nz + 1)
                            {
                                C->_nzmax = (nz_total + nz + 1) * 2;
                                C->_i->resize(C->_nzmax);
                                C->_x->resize(C->_nzmax);
                                ///printf("c.nz %d new %d\n", C->_nz, nz_total + nz + 1);
                            }
                            (*C->_i)[start_index_of_Ci + nz] = iIndexB;
                            ww[iIndexB] = jC + jA + 1;
                            xx[iIndexB] = x[iIndexB];
                            //ww[iIndexB] = j + 1;
                            //(*Y->_i)[start_index_of_Yi + nz] = iIndex;
                            //_x->push_back(val);
                            //x[iIndex] = val;
                            nz++;
                        }
                    }
                    else
                        x[iIndexB] = 0;
                }
            }

            // Copies results into C*j
            for(int i = start_index_of_Ci, k = 1 ; k <= nz ; ++i, ++k)
            {
                int iIndex = (*C->_i)[i];
                double val = xx[iIndex];
                //C->_i->push_back(iIndex);
                //C->_x->push_back(val);
                //(*C->_i)[i] = iIndex;
                (*C->_x)[i] = val;
                //printf("here2 i=%d iIndex=%d val=%g\n", i, iIndex, val);
            }
            nz_total += nz;
            (*C->_p)[jC + 1] = nz_total;
            // Now, copy x into Cx.
            start_index_of_Ci += nz;

            //break;
        }

        //std::cout << "size " << C->_i->size() << std::endl;
        ///std::cout << "nz_total " << nz_total << std::endl;
        ///std::cout << "x";
        ///for(int i = 0 ; i < 4; ++i)
        ///{
            //std::cout << " " << x[i];
            //std::cout << " " << (*C->_p)[i];
        ///    std::cout << " " << (*C->_i)[i];
        //}
        //std::cout << std::endl;

        ///for(int i = 0 ; i < nz_total ; ++i)
        ///{
        ///    std::cout << " " << (*C->_x)[i];
        ///}
        ///std::cout << std::endl;
        // Cleanup C
        //C->_i->resize(nz_total);
        //C->_x->resize(nz_total);
        C->_nz = nz_total;
        return C;
    }*/
    //----------------------------------------------------
    SparseMatrix *SparseMatrix::Transpose(const SparseMatrix *A)
    {
        SparseMatrix *C = new SparseMatrix(A->_n, A->_m, A->_nz);

        C->_nz = A->_nz;
        std::vector<int> w(C->_n, 0);     // workspace

        // Update each element of C->p per element of row-wise A
        /*for(int j = 0 ; j < A->_n ; ++j)
        {
            int i1A = (*A->_p)[j];         // Start index in _i
            int i2A = (*A->_p)[j + 1] - 1; // End index in _i

            for(int i = i1A ; i <= i2A ; ++i)
            {
                int iIndex = (*A->_i)[i];
                double val = (*A->_x)[i];

                for(unsigned int jj = iIndex + 1 ; jj < C->_p->size() ; ++jj)
                    (*C->_p)[jj] += 1;
            }
        }*/

        for(int i = 0 ; i < A->_i->size() ; ++i)
        {
            int iIndex = (*A->_i)[i];
            //double val = (*A->_x)[i];

            for(unsigned int jj = iIndex + 1 ; jj < C->_p->size() ; ++jj)
                (*C->_p)[jj] += 1;
        }

        // Exchange the rows and columns of A and store them into C matrix.
        for(int j = 0 ; j < A->_n ; ++j)
        {
            int i1A = (*A->_p)[j];         // Start index in _i
            int i2A = (*A->_p)[j + 1] - 1; // End index in _i

            for(int i = i1A ; i <= i2A ; ++i)
            {
                int iIndex = (*A->_i)[i];
                double val = (*A->_x)[i];

                int i1C = (*C->_p)[iIndex];         // Start index in _i
                int i2C = (*C->_p)[iIndex + 1] - 1; // End index in _i

                (*C->_i)[i1C + w[iIndex]] = j;
                (*C->_x)[i1C + w[iIndex]] = val;

                w[iIndex]++;
            }
        }

        return C;
    }
    //----------------------------------------------------
    void SparseMatrix::EliminateZeros()
    {
        int pos = 0;

        for(int j = 0 ; j < _n ; ++j)
        {
            int i1 = (*_p)[j];         // Start index in _i
            int i2 = (*_p)[j + 1] - 1; // End index in _i

            if(i2 < i1 || i2 < 0)
                continue;

            for(int i = i1 ; i <= i2 ; ++i)
            {
                //int iIndex = (*_i)[i];
                double val = (*_x)[i];

                if(val == 0)
                {
                    _i->erase(_i->begin() + pos);
                    _x->erase(_x->begin() + pos);

                    for(unsigned int jj = j + 1 ; jj < _p->size() ; ++jj)
                        (*_p)[jj] -= 1;

                    _nz--;
                    pos--;
                }
                else
                    pos++;
            }
        }
    }
    //----------------------------------------------------
    void SparseMatrix::Insert(int pos, int row, int column, double val)
    {
        if(_i->size() == 0)
        {
            _i->push_back(row);
            _x->push_back(val);
        }
        else
        {
            _i->insert(_i->begin() + pos, row);
            _x->insert(_x->begin() + pos, val);
        }

        for(unsigned int j = column + 1; j < _p->size() ; ++j)
            (*_p)[j] += 1;

        _nz++;

        //std::cout << "End Insert " << std::endl;
        //Print(); std::cout << "-------------" << std::endl;
    }
    //----------------------------------------------------
	void SparseMatrix::Remove(int pos, int column)
	{
		_i->erase(_i->begin() + pos);
        _x->erase(_x->begin() + pos);

        for(unsigned int j = column + 1; j < _p->size() ; ++j)
            (*_p)[j] -= 1;

        _nz--;
        //Print();
	}
	//----------------------------------------------------
    int SparseMatrix::SplashColumn(const SparseMatrix *X, SparseMatrix *Y, int column, int nz, int start_index_of_Yi, Operation o, std::vector<int> &w, std::vector<double> &x)
    {
        int j = column;

        int i1X = (*X->_p)[j];         // Start index in _i
        int i2X = (*X->_p)[j + 1] - 1; // End index in _i

        for(int i = i1X ; i <= i2X ; ++i)
        {
            int iIndex = (*X->_i)[i];
            double val = (*X->_x)[i];
            //printf("B[%d][%d]=%g\n", iIndex, j, val);
            if(w[iIndex] == j + 1) // it means that the item exists in w
            {
                if(o == addition)
                {
                    x[iIndex] += val; // If this addition results into an zero entry, we may
                                      // remove the zero elements from C matrix later through
                                      // a special-purpose function. We do not require it.
                                      //printf("here1 iIndex=%d w[iIndex]=%d\n", iIndex, w[iIndex]);
                }
                else if(o == multiplication)
                    x[iIndex] *= val;
            }
            else
            {
                w[iIndex] = j + 1;
                //C->_i->push_back(iIndex);
                (*Y->_i)[start_index_of_Yi + nz] = iIndex;
                //_x->push_back(val);
                x[iIndex] = val;
                nz++;
            }
        }

        return nz;
    }
    //----------------------------------------------------
    void SparseMatrix::Copy(const SparseMatrix *X, SparseMatrix *Y)
    {
        if(X->_n != Y->_n || X->_m != Y->_m)
            throw std::range_error("The number of rows and columns of the two matrices must be equal!");

        // Allocate matrix C such that it is equal to A.
        //int num_of_rows = X->_m;
        int num_of_columns = X->_n;
        Y->_i->resize(X->_nz);
        Y->_x->resize(X->_nz);
        memcpy(&(*Y->_p)[0], &(*X->_p)[0], (1 + num_of_columns) * sizeof(int));
        memcpy(&(*Y->_i)[0], &(*X->_i)[0], X->_nz * sizeof(int));
        memcpy(&(*Y->_x)[0], &(*X->_x)[0], X->_nz * sizeof(double));
        Y->_nz = X->_nz;
    }
    //----------------------------------------------------
    void SparseMatrix::Validate(int row, int column)
    {
        if(row < 0)
            throw std::range_error("row must be a positive number!");

        if(column < 0)
            throw std::range_error("column must be a positive number!");

        if(row > _m)
            throw std::range_error("row must be less than the number of rows!");

        if(column > _n)
            throw std::range_error("column must be less than the number of columns!");
    }
    //----------------------------------------------------
    void SparseMatrix::Print()
    {
        std::cout << "p";
        for(std::vector<int>::iterator it = _p->begin() ; it != _p->end() ; ++it)
        {
            int p = *it;
            std::cout << " " << p;
        }
        std::cout << std::endl;

        std::cout << "i";
        for(std::vector<int>::iterator it = _i->begin() ; it != _i->end() ; ++it)
        {
            int i = *it;
            std::cout << " " << i;
        }
        std::cout << std::endl;

        std::cout << "x";
        for(std::vector<double>::iterator it = _x->begin() ; it != _x->end() ; ++it)
        {
            double x = *it;
            std::cout << " " << x;
        }

        std::cout << std::endl;

        std::cout << "-------------" << std::endl;
    }
    //----------------------------------------------------
    void SparseMatrix::PrintMergedList(std::vector<int> &merged_idx, std::vector<double> &merged_data)
    {
        std::cout << "merged_idx";
        for(std::vector<int>::iterator it = merged_idx.begin() ; it != merged_idx.end() ; ++it)
        {
            int idx = *it;
            std::cout << " " << idx;
        }
        std::cout << std::endl;

        std::cout << "merged_data";
        for(std::vector<double>::iterator it = merged_data.begin() ; it != merged_data.end() ; ++it)
        {
            double data = *it;
            std::cout << " " << data;
        }
        std::cout << std::endl;
    }
    //----------------------------------------------------
};
//**************************************************************************************************************//
