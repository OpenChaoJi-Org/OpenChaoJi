/*
 *  Copyright (c) 2022 OpenChaoJi Org
 *
 *  Use of this source code is governed by an MIT-style
 *  license that can be found in the LICENSE file or at
 *  https://opensource.org/licenses/MIT.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 *  the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */

#include "ChaoJi_Trans.h"
#include "ChaoJi_Mem.h"


#define SIZES_OF_BLOCKS1	(8)
#define NUMS_OF_BLOCKS1		(10)

#define SIZES_OF_BLOCKS2	(16)
#define NUMS_OF_BLOCKS2		(5)

#define SIZES_OF_BLOCKS3	(32)
#define NUMS_OF_BLOCKS3		(5)

#define SIZES_OF_BLOCKS4	(64)
#define NUMS_OF_BLOCKS4		(10)



#define MEM_POOL_SIZES	((SIZES_OF_BLOCKS1*NUMS_OF_BLOCKS1)+\
					(SIZES_OF_BLOCKS2*NUMS_OF_BLOCKS2)+\
					(SIZES_OF_BLOCKS3*NUMS_OF_BLOCKS3)+\
					(SIZES_OF_BLOCKS4*NUMS_OF_BLOCKS4))

static uint8_t MemPool[MEM_POOL_SIZES];


static struct pMem	Blocks1[NUMS_OF_BLOCKS1];
static struct pMem	Blocks2[NUMS_OF_BLOCKS2];
static struct pMem	Blocks3[NUMS_OF_BLOCKS3];
static struct pMem	Blocks4[NUMS_OF_BLOCKS4];

void Mem_Init(void)
{
	int16_t i = 0;
	int16_t offset = 0;

	memset(MemPool,0,sizeof(MemPool));
	
	for (i = 0; i < NUMS_OF_BLOCKS1; i++)
	{
		Blocks1[i].payload = &(MemPool[offset+i*SIZES_OF_BLOCKS1]);
		Blocks1[i].used_flag = 0;
		Blocks1[i].len = SIZES_OF_BLOCKS1;
		offset +=SIZES_OF_BLOCKS1;
	}

	for (i = 0; i < NUMS_OF_BLOCKS2; i++)
	{
		Blocks2[i].payload = &(MemPool[offset+i*SIZES_OF_BLOCKS2]);
		Blocks2[i].used_flag = 0;
		Blocks2[i].len = SIZES_OF_BLOCKS2;
		offset +=SIZES_OF_BLOCKS2;
	}

	for (i = 0; i < NUMS_OF_BLOCKS3; i++)
	{
		Blocks3[i].payload = &(MemPool[offset+i*SIZES_OF_BLOCKS3]);
		Blocks3[i].used_flag = 0;
		Blocks3[i].len = SIZES_OF_BLOCKS3;
		offset +=SIZES_OF_BLOCKS3;
	}

	for (i = 0; i < NUMS_OF_BLOCKS4; i++)
	{
		Blocks4[i].payload = &(MemPool[offset+i*SIZES_OF_BLOCKS4]);
		Blocks4[i].used_flag = 0;
		Blocks4[i].len = SIZES_OF_BLOCKS4;
		offset +=SIZES_OF_BLOCKS4;
	}
}


/*allocate memory*/
void *Mem_get(uint16_t size)//TBD
{
	int16_t i;

	if (size <= SIZES_OF_BLOCKS1){
		for (i = 0; i < NUMS_OF_BLOCKS1; i++){
			if (Blocks1[i].used_flag == 0){
				Blocks1[i].used_flag = 1;
				return (&Blocks1[i]);
			}
		}
	}

	if (size <= SIZES_OF_BLOCKS2){
		for (i = 0; i < NUMS_OF_BLOCKS2; i++){
			if (Blocks2[i].used_flag == 0){
				Blocks2[i].used_flag = 1;
				return (&Blocks2[i]);
			}
		}
	}

	if (size <= SIZES_OF_BLOCKS3){
		for (i = 0; i < NUMS_OF_BLOCKS3; i++){
			if (Blocks3[i].used_flag == 0){
				Blocks3[i].used_flag = 1;
				return (&Blocks3[i]);
			}
		}
	}

	if (size <= SIZES_OF_BLOCKS4){
		for (i = 0; i < NUMS_OF_BLOCKS4; i++){
			if (Blocks4[i].used_flag == 0){
				Blocks4[i].used_flag = 1;
				return (&Blocks4[i]);
			}
		}
	}

	return NULL;
}

/*free memory*/
void Mem_free(void *pmem)//TBD
{
	struct pMem* p = (struct pMem*)pmem;
	
	p->used_flag = 0;
	memset(p->payload,0,p->len);//clear the buffer to 0
}

