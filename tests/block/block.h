
#ifndef TEST_BLOCK_H
#define TEST_BLOCK_H 1

#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <iostream>

class BlockTest : public OpenThreads::Thread
{
public:
	BlockTest() : mDone(false), mCount(0)
	{}

	~BlockTest()
	{
		cancel();
	}

	/*! block method */
	void block()
	{
		mOperator.block();
	}

	/*! override function */
	virtual int cancel()
	{
		mOperator.release();
		mDone = true;
		while (isRunning()) {
			OpenThreads::Thread::YieldCurrentThread();
		}
		return 0;
	}

	/*! override run */
	virtual void run()
	{
		do {
			std::cout << "(" << mCount << ")" << std::endl;
			mCount++;

			if (mCount == 10) {
				mOperator.release();
				mOperator.reset();
				mOperator.block();
			}
			OpenThreads::Thread::microSleep(150000L);
		} while (!mDone);
	}

protected:
	/*! done over */
	bool mDone; 
	unsigned long mCount;
	OpenThreads::Block mOperator;
};


#endif // TEST_BLOCK_H

