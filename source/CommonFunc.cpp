
//
//  îƒópä÷êî
//
//  Written by Otachan
//  http://otachan.com/
//

#define	STRICT

#include <windows.h>
#include <mbctype.h>

#include "WSL/WSL.h"

#include "CommonFunc.h"

void
CutPathFileName(
			const char* FullFileName,
			char* FileNamePath,
			const size_t FileNamePathSize,
			char* FileName,
			const size_t FileNameSize)
{
	unsigned char	Str;
	size_t	cnt = 0;
	size_t	BackSlashCnt = -1;
	bool	Kanji = false;

	while((Str = *(FullFileName + cnt)) != '\0') {
		if(!Kanji) {
			if(_ismbblead(Str)) {
				Kanji = true;
			} else {
				if((Str == '\\') || (Str == '/')) {
					BackSlashCnt = cnt;
				}
			}
		} else {
			Kanji = false;
		}
		cnt++;
	}

	BackSlashCnt++;

	if(FileNamePath) {
		const size_t	CopySize = Min(BackSlashCnt, FileNamePathSize - 1);

		memcpy(FileNamePath, FullFileName, CopySize);
		*(FileNamePath + CopySize) = '\0';
	}

	if(FileName) {
		strcpy_s(FileName, FileNameSize, FullFileName + BackSlashCnt);
	}
}

