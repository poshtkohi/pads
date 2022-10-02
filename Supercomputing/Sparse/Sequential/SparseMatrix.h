/**
    #define meta ...
    printf("%s\n", meta);
**/

/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Supercomputing_Sparse_Sequential_SparseMatrix_h__
#define __Supercomputing_Sparse_Sequential_SparseMatrix_h__


#include <string.h>
#include <iostream>
#include <vector>
#include <map>

enum Operation { addition, multiplication };

using namespace std;

//**************************************************************************************************************//
namespace Supercomputing::Sparse::Sequential
{
    //----------------------------------------------------
    //template<typename T>
    // C++ implementation of sparse matrix using CCS (Compressed Column Storage) format.
    class SparseMatrix// : public Object
    {
        /*---------------------fields-----------------*/
        // Number of rows
        public: int _m;
        // Number of columns
        public: int _n;
        // Number of non-zero elements
        public: int _nz;
        // Maximum number of non-zero elements
        public: int _nzmax;
        // Column pointers
        private: std::vector<int> *_p;
        // Row indices
        private: std::vector<int> *_i;
        // Numerical values
        private: std::vector<double> *_x;
        /*---------------------methods----------------*/
        // SparseMatrix Class constructors.
        private: SparseMatrix(unsigned int m, unsigned int n, unsigned int nzmax);
        public: SparseMatrix(unsigned int m, unsigned int n);
        // Creates a sparse matrix using the basic CSC representation.
        public: SparseMatrix(int m, int n, const std::vector<int> &p, const std::vector<int> &i, const std::vector<double> &x);
        // SparseMatrix Class destructor.
        public: ~SparseMatrix();
        // Sets a value at a specified row and column
        public: void Set(int row, int column, double val);
        // Gets a value at a specified row and column
        public: double Get(int row, int column);
        // Adds two matrices and returns the resultant.
        public: static SparseMatrix *Add(const SparseMatrix *A, const SparseMatrix *B);
        // Adds two matrices and returns the resultant.
        public: static SparseMatrix *Multiply(const SparseMatrix *A, const SparseMatrix *B);
        // Multiplies A matrix by V vector and stores the result into r.
        public: static void MultiplyByVector(const SparseMatrix *A, const std::vector<double> &v, std::vector<double> &r);
        // Transposes the matrix A.
        public: static SparseMatrix *Transpose(const SparseMatrix *A);
        // Inserts a value at a specified row and column
        private: void Insert(int pos, int row, int column, double val);
        // Removes an element from matrix at a specified row and column
        private: void Remove(int pos, int column);
        // Checks the validity of Set/Get methods.
        private: void Validate(int row, int column);
        // Eliminates zero elements of the matrix instance.
        public: void EliminateZeros();
        // Copies sparse matrix X into Y.
        private: static void Copy(const SparseMatrix *X, SparseMatrix *Y);
        // Prepares/performs column-wise operations on X matrix where the results are stored in Y matrix. w and x stands for workspace variables.
        private: static int SplashColumn(const SparseMatrix *X, SparseMatrix *Y, int column, int nz, int start_index_of_Yi, Operation o, std::vector<int> &w, std::vector<double> &x);
        // Gets the underlying data structure information.
        public: inline int GetM() { return this->_m; }
        public: inline int GetN() { return this->_n; }
        public: inline int GetNNZ() { return this->_nz; }
        public: inline const std::vector<int> &GetP() { return *this->_p; }
        public: inline const std::vector<int> &GetI() { return *this->_i; }
        public: inline const std::vector<double> &GetX() { return *this->_x; }
        public: void Print();
        private: static void PrintMergedList(std::vector<int> &merged_idx, std::vector<double> &merged_data);
        public: inline friend ::std::ostream &operator<<(::std::ostream &stream, SparseMatrix &matrix)
        {
            for (int i = 0 ; i < matrix._m ; i++)
            {
                for (int j = 0 ; j < matrix._n ; j++)
                {
                    if (j != 0)
                    {
                        stream << " ";
                    }

                    stream << matrix.Get(i, j);
                }

                if (i < matrix._m)
                {
                    stream << std::endl;
                }
            }

            return stream;
        }
    };
	//----------------------------------------------------
};

//**************************************************************************************************************//

#endif
