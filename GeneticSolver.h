//////////////////////////////
// GeneticSolver.h          //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#ifndef gws_GeneticSolver_h
#define gws_GeneticSolver_h

#include "Solver.h"

#ifdef __APPLE__
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif

#include <string>

namespace gws
{
	class Path;
	class Puzzle;
	
	///////////////////////////////////////////////////////////////////////////
	/// \class GeneticSolver
	/// \brief Puzzle solver that executes a genetic algorithm on the GPU
	///////////////////////////////////////////////////////////////////////////
	class GeneticSolver : public Solver
	{
	public:
		GeneticSolver(size_t puzzleWidth, size_t puzzleHeight, size_t populationSize, size_t maxIterations);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Virtual destructor
		///////////////////////////////////////////////////////////////////////
		virtual ~GeneticSolver();
		
		// prevent creating copies of the solver
		GeneticSolver(const GeneticSolver& other) = delete;
		GeneticSolver& operator=(const GeneticSolver* other) = delete;
		
		///////////////////////////////////////////////////////////////////////
		/// \copydoc Solver::solvePuzzle()
		///////////////////////////////////////////////////////////////////////
		virtual bool solvePuzzle(const Puzzle& puzzle, Path& path);
		
	private:
		size_t m_populationSize;
		size_t m_maxIterations;
		
		size_t m_numPuzzlePoints;
		size_t m_numPuzzleEdges;
		size_t m_numPuzzleSpaces;
		
		cl_int m_lastErrNum;
		
		cl_device_id m_deviceID;
		cl_context m_context;
		cl_command_queue m_queue;
		
		cl_program m_program;
		cl_kernel m_generateKernel;
		cl_kernel m_evaluateKernel;
		cl_kernel m_crossoverMutateKernel;
		
		cl_mem m_puzzlePointBuffer;
		cl_mem m_puzzleEdgeBuffer;
		cl_mem m_puzzleSpaceBuffer;
		
		cl_mem m_populationBuffer1;
		cl_mem m_populationBuffer2;
		cl_mem m_fitnessBuffer;
		
		void runGenerateKernel(cl_mem outputPopulationBuffer);
		bool runEvaluationKernel(cl_mem inputPopulationBuffer);
		void runCrossoverMutationKernel(cl_mem inputPopulationBuffer, cl_mem outputPopulationBuffer);
		
		void initDeviceContextAndQueue();
		void initProgramAndKernels();
		void initBuffers();
		
		void cleanup();
		
		void checkLastErr(const std::string& name) const;
	};
}

#endif