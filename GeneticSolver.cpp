//////////////////////////////
// GeneticSolver.cpp        //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "GeneticSolver.h"

#include "Path.h"
#include "Puzzle.h"

#include <climits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

// selected to fit under local memory size constraints
#define LOCAL_WORK_SIZE 32

namespace gws
{
	GeneticSolver::GeneticSolver(size_t puzzleWidth, size_t puzzleHeight, size_t populationSize, size_t maxIterations, float crossoverRate, float mutationRate, unsigned int seed)
		: m_puzzleWidth(puzzleWidth),
		  m_puzzleHeight(puzzleHeight),
		  m_numPuzzlePoints(Puzzle::getNumPoints(puzzleWidth, puzzleHeight)),
		  m_numPuzzleEdges(Puzzle::getNumEdges(puzzleWidth, puzzleHeight)),
		  m_numPuzzleSpaces(Puzzle::getNumSpaces(puzzleWidth, puzzleHeight)),
		  m_populationSize(populationSize),
		  m_maxIterations(maxIterations),
		  m_crossoverRate(crossoverRate),
		  m_mutationRate(mutationRate),
		  m_seed(seed)
	{
		// initialize OpenCL components
		initDeviceContextAndQueue();
		initBuffers();
		initProgramAndKernels();
	}
	
	GeneticSolver::~GeneticSolver()
	{
		// cleanup OpenCL components
		cleanup();
	}
	
	bool GeneticSolver::solvePuzzle(const Puzzle& puzzle, Path& path)
	{
		// transfer puzzle data to device
		transferPuzzleData(puzzle);
		
		// find the max fitness value of the puzzle
		int maxPuzzleFitness = calcMaxFitness(puzzle);
		std::cout << "Max puzzle fitness is " << maxPuzzleFitness << std::endl;
		// seed random number generator
		srand(m_seed);
		
		// randomly generate initial population (TODO: move to deivce)
		size_t totalPopulationBytes = m_populationSize*m_numPuzzlePoints;
		unsigned char* population = new unsigned char[totalPopulationBytes];
		for (size_t i = 0; i < totalPopulationBytes; ++i) {
			population[i] = (unsigned char)(rand() % UCHAR_MAX);
		}
		
		// iterate over each generation until a correct solution is found or max iterations is reached
		bool solutionFound = false;
		m_numIterations = 0;
		int* fitness = new int[m_populationSize];
		unsigned int* startPoints = new unsigned int[m_populationSize];
		char* paths = new char[totalPopulationBytes];
		size_t maxMember = 0;
		while (!solutionFound && m_numIterations < m_maxIterations) {
			runEvaluationKernel(population, fitness, startPoints, paths);
			
			// check for correct solution (break early if solution found)
			size_t currentMember = 0;
			int totalFitness = 0;
			int currentFitness = 0;
			int currentMinFitness = INT_MAX;
			int currentMaxFitness = INT_MIN;
			while (!solutionFound && currentMember < m_populationSize) {
				currentFitness = fitness[currentMember];
				if (currentFitness == maxPuzzleFitness) {
					fillPath(paths, startPoints, currentMember, m_numPuzzlePoints, path);
					solutionFound = true;
				}
				
				// collect fitness metrics to assist with crossover phase
				totalFitness += currentFitness;
				if (currentFitness < currentMinFitness) {
					currentMinFitness = currentFitness;
				}
				if (currentFitness > currentMaxFitness) {
					currentMaxFitness = currentFitness;
					maxMember = currentMember;
				}
				
				++currentMember;
			}
			
			if (m_numIterations % 100 == 0) {
				std::cout << m_numIterations << " | current max: " << currentMaxFitness << " | path: " << paths + (maxMember*m_numPuzzlePoints) << std::endl;
			}
			
			// generate next population via crossover/mutation (TODO: move to device)
			if (!solutionFound) {
				// normalize fitness so that all values are positive
				// (need at least 1 to have chance of being chosen)
				totalFitness = totalFitness - currentMinFitness + 1;
				
				// randomly crossover population according to crossover rate
				for (size_t i = 0; i < m_populationSize; ++i) {
					float crossoverDecision = (float)rand()/RAND_MAX;
					if (crossoverDecision <= m_crossoverRate) {
						size_t mate = 0;
						float fitnessCount = 0.0f;
						float fitnessDecision = ((float)rand()/RAND_MAX)*totalFitness;
						bool mateFound = false;
						while (!mateFound && mate < m_populationSize) {
							// normalize fitness so that all values are positive
							// (need at least 1 to have chance of being chosen)
							fitnessCount += fitness[mate] - currentMinFitness + 1;
							if (fitnessCount >= fitnessDecision) {
								mateFound = true;
							}
							else {
								++mate;
							}
						}
						
						// perform single-point crossover
						size_t crossoverPoint = rand()%m_numPuzzlePoints;
						for (size_t j = 0; j < m_numPuzzlePoints; ++j) {
							size_t destinationIndex = i*m_numPuzzlePoints + j;
							size_t parentIndex1 = destinationIndex;
							size_t parentIndex2 = mate*m_numPuzzlePoints + j;
							
							// "mate" is more likely to have a high fitness, so use it as the starting parent
							if (j > crossoverPoint) {
								population[destinationIndex] = population[parentIndex1];
							}
							else {
								population[destinationIndex] = population[parentIndex2];
							}
						}
					}
				}
				
				// randomly mutate population according to mutation rate
				for (size_t i = 0; i < totalPopulationBytes; ++i) {
					float mutationDecision = (float)rand()/RAND_MAX;
					if (mutationDecision <= m_mutationRate) {
						population[i] = (unsigned char)(rand() % UCHAR_MAX);
					}
				}
			}
			
			++m_numIterations;
		}
		
		delete [] population;
		delete [] fitness;
		delete [] startPoints;
		delete [] paths;
		
		return solutionFound;
	}
	
	size_t GeneticSolver::getNumIterations() const
	{
		return m_numIterations;
	}
	
	int GeneticSolver::calcMaxFitness(const Puzzle& puzzle) const
	{
		// start with 1 fitness point for reaching the end
		int maxFitness = 1;
		
		// count dot constraints (each dot is 1 fitness point)
		for (size_t i = 0; i < m_numPuzzlePoints; ++i) {
			if (puzzle.getPointValue(i) == PointValue::DOT) {
				++maxFitness;
			}
		}
		for (size_t i = 0; i < m_numPuzzleEdges; ++i) {
			if (puzzle.getEdgeValue(i) == EdgeValue::DOT) {
				++maxFitness;
			}
		}
		
		// space partition constraints are counted as negative
		// fitness for violations so no need to check here
		return maxFitness;
	}
	
	void GeneticSolver::fillPath(const char* paths, const unsigned int* startPoints, size_t member, size_t maxLength, Path& path) const
	{
		// clear out path
		path.clear();
		
		// set start point
		path.setStartPointIndex(startPoints[member]);
		
		// extract path data
		size_t startIndex = member*maxLength;
		size_t i = 0;
		bool morePath = true;
		while (i < maxLength && morePath) {
			MoveValue value = (MoveValue)paths[startIndex + i];
			if (value != MoveValue::NONE) {
				path.addMove(value);
			}
			else {
				morePath = false;
			}
			
			++i;
		}
	}
	
	void GeneticSolver::initDeviceContextAndQueue()
	{
		cl_uint numPlatforms;
		cl_uint numDevices;
		
		// first, select an OpenCL platform to run on
		m_lastErrNum = clGetPlatformIDs(0, NULL, &numPlatforms);
		if (m_lastErrNum == CL_SUCCESS && numPlatforms <= 0) {
			m_lastErrNum = -1;
		}
		checkLastErr("clGetPlatformIDs");
		
		cl_platform_id* platformIDs = (cl_platform_id *)alloca(sizeof(cl_platform_id)*numPlatforms);
		
		m_lastErrNum = clGetPlatformIDs(numPlatforms, platformIDs, NULL);
		checkLastErr("clGetPlatformIDs");
		
		// iterate through the list of platforms until we find one that supports a GPU device,
		// otherwise fail with an error
		cl_uint i;
		cl_device_id* deviceIDs = NULL;
		for (i = 0; i < numPlatforms; ++i) {
			m_lastErrNum = clGetDeviceIDs(
				platformIDs[i],
				CL_DEVICE_TYPE_GPU,
				0,
				NULL,
				&numDevices);
			if (m_lastErrNum != CL_SUCCESS && m_lastErrNum != CL_DEVICE_NOT_FOUND) {
				checkLastErr("clGetDeviceIDs");
			}
			else if (numDevices > 0) {
				deviceIDs = (cl_device_id *)alloca(sizeof(cl_device_id) * numDevices);
				m_lastErrNum = clGetDeviceIDs(
					platformIDs[i],
					CL_DEVICE_TYPE_GPU,
					numDevices,
					&deviceIDs[0],
					NULL);
				checkLastErr("clGetDeviceIDs");
				
				m_deviceID = deviceIDs[0];
				break;
			}
		}
		
		// next, create an OpenCL context on the selected platform
		cl_context_properties contextProperties[] =
		{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)platformIDs[i],
			0
		};
		
		m_context = clCreateContext(
			contextProperties,
			numDevices,
			deviceIDs,
			NULL,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateContext");
		
		// create the command queue
		m_queue = clCreateCommandQueue(
			m_context,
			m_deviceID,
			0,
			&m_lastErrNum);
		checkLastErr("clCreateCommandQueue");
	}
	
	void GeneticSolver::initBuffers()
	{
		// create read-only buffers to hold constant puzzle data
		m_puzzlePointBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzlePoints,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_puzzleEdgeBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzleEdges,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_puzzleSpaceBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzleSpaces,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		// create buffers to store populations (path solutions) and fitness results
		// (max path length is the number of puzzle points)
		m_populationBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(unsigned char)*m_numPuzzlePoints*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_fitnessBuffer = clCreateBuffer(
			m_context,
			CL_MEM_WRITE_ONLY,
			sizeof(int)*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_startPointBuffer = clCreateBuffer(
			m_context,
			CL_MEM_WRITE_ONLY,
			sizeof(unsigned int)*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_pathsBuffer = clCreateBuffer(
			m_context,
			CL_MEM_WRITE_ONLY,
			sizeof(char)*m_numPuzzlePoints*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
	}
	
	void GeneticSolver::initProgramAndKernels()
	{
		std::ifstream srcFile("GeneticSolver.cl");
		if (!srcFile.is_open()) {
			m_lastErrNum = -1;
		}
		checkLastErr("Reading GeneticSolver.cl");
		
		std::string srcProg(
			std::istreambuf_iterator<char>(srcFile),
			(std::istreambuf_iterator<char>()));
		
		const char* src = srcProg.c_str();
		size_t length = srcProg.length();
		
		// create program from source
		m_program = clCreateProgramWithSource(
			m_context,
			1,
			&src,
			&length,
			&m_lastErrNum);
		checkLastErr("clCreateProgramWithSource");
		
		// build program
		m_lastErrNum = clBuildProgram(
			m_program,
			1,
			&m_deviceID,
			NULL,
			NULL,
			NULL);
		if (m_lastErrNum != CL_SUCCESS) {
			// determine the reason for the error
			char buildLog[16384];
			clGetProgramBuildInfo(
				m_program,
				m_deviceID,
				CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog),
				buildLog,
				NULL);
			
			std::cerr << "Error in OpenCL C source: " << std::endl;
			std::cerr << buildLog;
			checkLastErr("clBuildProgram");
		}
		
		// create kernel
		m_evaluateKernel = clCreateKernel(
			m_program,
			"evaluatePopulation",
			&m_lastErrNum);
		checkLastErr("clCreateKernel");
		
		// set input arguments
		m_lastErrNum = clSetKernelArg(m_evaluateKernel, 0, sizeof(cl_mem), &m_populationBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 1, sizeof(unsigned int), &m_populationSize);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 2, sizeof(unsigned int), &m_numPuzzlePoints);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 3, sizeof(cl_mem), &m_puzzlePointBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 4, sizeof(cl_mem), &m_puzzleEdgeBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 5, sizeof(cl_mem), &m_puzzleSpaceBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 6, sizeof(unsigned int), &m_numPuzzlePoints);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 7, sizeof(unsigned int), &m_numPuzzleEdges);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 8, sizeof(unsigned int), &m_numPuzzleSpaces);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 9, sizeof(unsigned int), &m_puzzleWidth);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 10, sizeof(unsigned int), &m_puzzleHeight);
		
		// set sizes of local memory scratch space arrays for each work group
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 11, sizeof(bool)*m_numPuzzlePoints*LOCAL_WORK_SIZE, NULL);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 12, sizeof(bool)*m_numPuzzleEdges*LOCAL_WORK_SIZE, NULL);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 13, sizeof(char)*m_numPuzzleSpaces*LOCAL_WORK_SIZE, NULL);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 14, sizeof(bool)*m_numPuzzleSpaces*LOCAL_WORK_SIZE, NULL);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 15, sizeof(bool)*m_numPuzzleSpaces*LOCAL_WORK_SIZE, NULL);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 16, sizeof(unsigned char)*m_numPuzzleSpaces*LOCAL_WORK_SIZE, NULL);
		
		// set output arguments
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 17, sizeof(cl_mem), &m_fitnessBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 18, sizeof(cl_mem), &m_startPointBuffer);
		m_lastErrNum |= clSetKernelArg(m_evaluateKernel, 19, sizeof(cl_mem), &m_pathsBuffer);
		
		checkLastErr("clSetKernelArg");
	}
	
	void GeneticSolver::transferPuzzleData(const Puzzle& puzzle)
	{
		m_lastErrNum = clEnqueueWriteBuffer(
			m_queue,
			m_puzzlePointBuffer,
			CL_FALSE,
			0,
			sizeof(char)*m_numPuzzlePoints,
			(void*)puzzle.getPointData(),
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueWriteBuffer");
		
		m_lastErrNum = clEnqueueWriteBuffer(
			m_queue,
			m_puzzleEdgeBuffer,
			CL_FALSE,
			0,
			sizeof(char)*m_numPuzzleEdges,
			(void*)puzzle.getEdgeData(),
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueWriteBuffer");
		
		m_lastErrNum = clEnqueueWriteBuffer(
			m_queue,
			m_puzzleSpaceBuffer,
			CL_FALSE,
			0,
			sizeof(char)*m_numPuzzleSpaces,
			(void*)puzzle.getSpaceData(),
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueWriteBuffer");
	}
	
	void GeneticSolver::runEvaluationKernel(const unsigned char* population, int* fitness, unsigned int* startPoints, char* paths)
	{
		// copy current population to device
		m_lastErrNum = clEnqueueWriteBuffer(
			m_queue,
			m_populationBuffer,
			CL_FALSE,
			0,
			sizeof(unsigned char)*m_populationSize*m_numPuzzlePoints,
			(void*)population,
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueWriteBuffer");
		
		// run kernel
		size_t globalWorkSize[1] = { m_populationSize };
		size_t localWorkSize[1] = { LOCAL_WORK_SIZE };
		m_lastErrNum = clEnqueueNDRangeKernel(
			m_queue,
			m_evaluateKernel,
			1,
			NULL,
			globalWorkSize,
			localWorkSize,
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueNDRangeKernel");
		
		// copy fitness and path results back to host
		m_lastErrNum = clEnqueueReadBuffer(
			m_queue,
			m_fitnessBuffer,
			CL_FALSE,
			0,
			sizeof(int)*m_populationSize,
			(void*)fitness,
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueReadBuffer");
		
		m_lastErrNum = clEnqueueReadBuffer(
			m_queue,
			m_startPointBuffer,
			CL_FALSE,
			0,
			sizeof(unsigned int)*m_populationSize,
			(void*)startPoints,
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueReadBuffer");
		
		m_lastErrNum = clEnqueueReadBuffer(
			m_queue,
			m_pathsBuffer,
			CL_TRUE,
			0,
			sizeof(char)*m_populationSize*m_numPuzzlePoints,
			(void*)paths,
			0,
			NULL,
			NULL);
		checkLastErr("clEnqueueReadBuffer");
	}
	
	void GeneticSolver::cleanup()
	{
		// clean up memory buffers
		if (m_puzzlePointBuffer != 0) {
			clReleaseMemObject(m_puzzlePointBuffer);
		}
		if (m_puzzleEdgeBuffer != 0) {
			clReleaseMemObject(m_puzzleEdgeBuffer);
		}
		if (m_puzzleSpaceBuffer != 0) {
			clReleaseMemObject(m_puzzleSpaceBuffer);
		}
		if (m_populationBuffer != 0) {
			clReleaseMemObject(m_populationBuffer);
		}
		if (m_fitnessBuffer != 0) {
			clReleaseMemObject(m_fitnessBuffer);
		}
		if (m_pathsBuffer != 0) {
			clReleaseMemObject(m_pathsBuffer);
		}
		
		// clean up command queue
		if (m_queue != 0) {
			clReleaseCommandQueue(m_queue);
		}
		
		// clean up kernel and program
		if (m_evaluateKernel != 0) {
			clReleaseKernel(m_evaluateKernel);
		}
		if (m_program != 0) {
			clReleaseProgram(m_program);
		}
		
		// clean up context
		if (m_context != 0) {
			clReleaseContext(m_context);
		}
	}
	
	void GeneticSolver::checkLastErr(const std::string& name) const
	{
		if (m_lastErrNum != CL_SUCCESS) {
			std::ostringstream oss;
			oss << "ERROR: " <<  name << " (" << m_lastErrNum << ")" << std::endl;
			
			throw std::runtime_error(oss.str());
		}
	}
}