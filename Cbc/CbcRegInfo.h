#ifndef __CBCREGINFO_H__
#define __CBCREGINFO_H__
#include <TROOT.h>
#include <map>
#include <vector>
#include <iostream>
/*
	input file
		Name	Page	Address	DefVal	Value0

*/
namespace Cbc{

	class CbcRegItem{

		public:
			CbcRegItem():fFeId(0), fCbcId(0), fName(), fDefVal(0), fValue0(0), fWrittenValue(0), fReadValue(0), fWriteFailed(0){}
			CbcRegItem( UInt_t pFeId, UInt_t pCbcId, const char *pName, UInt_t pPage, UInt_t pAddr, UInt_t pDefVal, UInt_t pValue0, Bool_t pWriteFailed=0 ): 
				fFeId(pFeId), fCbcId(pCbcId), fName(pName), fPage(pPage), fAddress(pAddr), fDefVal(pDefVal), fValue0(pValue0), fWrittenValue(pValue0), fReadValue(0), fWriteFailed(pWriteFailed){}
			~CbcRegItem(){}

			UInt_t FeId()const{ return fFeId; }
			UInt_t CbcId()const{ return fCbcId; }
			const std::string &Name()const{ return fName; } 
			UInt_t Page()const{ return fPage; }
			UInt_t Address()const{ return fAddress; }
			UInt_t DefVal()const{ return fDefVal; }
			UInt_t Value0()const{ return fValue0; }         //The initial value
			UInt_t ReadValue()const{ return fReadValue; }       //Read value
			UInt_t WrittenValue()const{ return fWrittenValue; }   //Written value
			Bool_t WriteFailed()const{ return fWriteFailed; }

			void SetFeId( UInt_t pFe ){ fFeId = pFe; }
			void SetCbcId( UInt_t pCbc ){ fCbcId = pCbc; }
			void SetName( const char * pName ){ fName = pName; }
			void SetPage( UInt_t pPage ){ fPage = pPage; }
			void SetAddress( UInt_t pAddr ){ fAddress = pAddr; }
			void SetDefVal( UInt_t pDefVal ){ fDefVal = pDefVal; }
			void SetValue0( UInt_t pValue0 ){ fValue0 = pValue0; }
			void SetWrittenValue( UInt_t pWrittenValue ){ fWrittenValue = pWrittenValue; }
			void SetReadValue( UInt_t pReadValue );

		private:
			UInt_t fFeId;
			UInt_t fCbcId;
			std::string fName;
			UInt_t fPage;
			UInt_t fAddress;
			UInt_t fDefVal;
			UInt_t fValue0;          //Original register value in the setting file
			UInt_t fWrittenValue;    //Written register value 
			UInt_t fReadValue;       //Read register value
			Bool_t fWriteFailed;   //Set to true if value != value0.
	};

	class CbcRegList: public std::vector<CbcRegItem *>{

		friend class FeCbcRegMap;

		public:
		CbcRegList( UInt_t pFeId=0, UInt_t pCbcId = 0 ): std::vector<CbcRegItem *>(0), 
		fFeId(pFeId), fCbcId(pCbcId), fSettingFileName( Form("settings/FE%dCBC%d.txt", fFeId, fCbcId ) ){}
		~CbcRegList(){ Clear(); }

		const CbcRegItem *GetRegItem( UInt_t pPage, UInt_t pAddr )const; 
		const CbcRegItem *GetRegItem( const std::string &pName )const; 
		UInt_t GetValue0( UInt_t pPage, UInt_t pAddr )const;
		UInt_t GetValue0( const std::string &pName )const;
		UInt_t GetWrittenValue( UInt_t pPage, UInt_t pAddr )const;
		UInt_t GetWrittenValue( const std::string &pName )const;
		UInt_t GetReadValue( UInt_t pPage, UInt_t pAddr )const;
		UInt_t GetReadValue( const std::string &pName )const;
		const std::string &GetFileName()const{ return fSettingFileName; }
		UInt_t FeId()const{ return fFeId; }
		UInt_t CbcId()const{ return fCbcId; }

		void Clear();
		void AddRegItem( CbcRegItem *pCbcRegItem ){ push_back( pCbcRegItem ); }
		void SetValue0( UInt_t pPage, UInt_t pAddr, UInt_t pReadValue0 );
		void SetValue0( const std::string &pName, UInt_t pReadValue0 );
		void SetWrittenValue( UInt_t pPage, UInt_t pAddr, UInt_t pValue );
		void SetWrittenValue( const std::string &pName, UInt_t pValue );
		void SetReadValue( UInt_t pPage, UInt_t pAddr, UInt_t pReadValue );
		void SetReadValue( const std::string &pName, UInt_t pReadValue );
		void SetFileName( const std::string &pFileName ){ fSettingFileName = pFileName; }

		private:
		CbcRegItem *getRegItem( const std::string &pName ); 
		CbcRegItem *getRegItem( UInt_t pPage, UInt_t pAddr ); 

		UInt_t fFeId;
		UInt_t fCbcId;
		std::string fSettingFileName;
	};

	class FeCbcRegMap: public std::map<UInt_t, CbcRegList>{ 

		friend class CbcRegMap;

		public:
		FeCbcRegMap(UInt_t pFeId=0): fFeId(pFeId){}
		~FeCbcRegMap(){ Clear(); }

		const CbcRegList *GetCbcRegList( UInt_t pCbc )const;
		const CbcRegItem *GetCbcRegItem( UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
		const CbcRegItem *GetCbcRegItem( UInt_t pCbc, const std::string &pName )const;
		UInt_t GetReadValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
		UInt_t GetReadValue( UInt_t pCbc, const std::string &pName )const;

		void Clear();
		void AddCbc( UInt_t pCbc, CbcRegList pCC=CbcRegList() ); 
		void AddItem( UInt_t pCbc, CbcRegItem *pItem ); 
		void SetValue0( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue0 );
		void SetValue0( UInt_t pCbc, const std::string &pName, UInt_t pValue0 );
		void SetWrittenValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue );
		void SetWrittenValue( UInt_t pCbc, const std::string &pName, UInt_t pValue );
		void SetReadValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pReadValue );
		void SetReadValue( UInt_t pCbc, const std::string &pName, UInt_t pReadValue );

		private:
		CbcRegList *getCbcRegList( UInt_t pCbc );
		CbcRegItem *getCbcRegItem( UInt_t pCbc, UInt_t pPage, UInt_t pAddr );
		CbcRegItem *getCbcRegItem( UInt_t pCbc, const std::string &pName );

		UInt_t fFeId;
	};

	class CbcRegMap : public std::map<UInt_t, FeCbcRegMap >{ 

		public:
			CbcRegMap(){}
			~CbcRegMap(){ Clear(); }

			const FeCbcRegMap *GetFeCbcRegMap( UInt_t pFe )const;
			const CbcRegList *GetCbcRegList( UInt_t pFe, UInt_t pCbc )const;
			const CbcRegItem *GetCbcRegItem( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
			const CbcRegItem *GetCbcRegItem( UInt_t pFe, UInt_t pCbc, const std::string &pName )const;
			UInt_t GetValue0( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
			UInt_t GetValue0( UInt_t pFE, UInt_t pCbc, const std::string &pName )const;
			UInt_t GetWrittenValue( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
			UInt_t GetWrittenValue( UInt_t pFE, UInt_t pCbc, const std::string &pName )const;
			UInt_t GetReadValue( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const;
			UInt_t GetReadValue( UInt_t pFE, UInt_t pCbc, const std::string &pName )const;

			void Clear();
			void Reset( UInt_t pNFe, UInt_t pNCbc );
			void SetCbcRegSettingFileName( UInt_t pFeId, UInt_t pCbcId, const std::string &pName );
			void AddFe( UInt_t pFe );
			void AddItem( UInt_t pFe, UInt_t pCbc, CbcRegItem *pItem ); 
			const CbcRegItem *SetValue0( UInt_t pFE, UInt_t pCbc, const std::string &pName, UInt_t pValue0 );
			const CbcRegItem *SetValue0( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue0 );
			const CbcRegItem *SetWrittenValue( UInt_t pFE, UInt_t pCbc, const std::string &pName, UInt_t pWrittenValue );
			const CbcRegItem *SetWrittenValue( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pWrittenValue );
			const CbcRegItem *SetReadValue( UInt_t pFE, UInt_t pCbc, const std::string &pName, UInt_t pReadValue );
			const CbcRegItem *SetReadValue( UInt_t pFE, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pReadValue );
			void ClearRegisters();
			void ClearRegisters( UInt_t pFe, UInt_t pCbc );

		private:
			FeCbcRegMap *getFeCbcRegMap( UInt_t pFe );
			CbcRegList *getCbcRegList( UInt_t pFe, UInt_t pCbc );
			CbcRegItem *getCbcRegItem( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr );
			CbcRegItem *getCbcRegItem( UInt_t pFe, UInt_t pCbc, const std::string &pName );
	};
}

#endif

