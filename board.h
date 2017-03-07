#ifndef __board__
#define __board__
#include<cctype>
#include<cstdio>
#include<utility>
#include<algorithm>
#include<iostream>

const bool BLACK = false ;
const bool WHITE = true ;

class board{
	// my variables
	// black = X (0)
	// white = O (1)
	unsigned long long black, white ;
	bool my_tile, op_tile ;
	int pass ;

	public:

	// Construct board.
	// my_tile = BLACK
	// op_tile = WHITE
	constexpr board(): black(34628173824), white(68853694464), my_tile(false), op_tile(true), pass(0) {}

	// Input: b = black board
	// 	  w = white board
	// 	  turn = whose turn now
	// 	  _pass = pass count
	board(const unsigned long long b, const unsigned long long w, const bool turn, const int _pass){
		black = b ;
		white = w ;
		pass = _pass ;

		if( turn == BLACK ){
			my_tile = BLACK ;
			op_tile = WHITE ;
		}
		else if( turn == WHITE ){
			my_tile = WHITE ;
			op_tile = BLACK ;
		}
		else
			*this = board() ;
	}

	// Construct board from code.
	board(const char*st,const char*ed){
		black = 0 ;
		white = 0 ;
		if(ed-st==66&&std::all_of(st,ed,isdigit)){
			for(int i=0;i<64;i++){
				if( (*(st++)-'0')%3 == 1 ) // black
					black |= (1ULL<<i) ;
				else if( (*(st++)-'0')%3 == 2 ) // white
					white |= (1ULL<<i) ;
			}
			my_tile = 2-(*(st++)-'0')%2 - 1;
			op_tile = !my_tile ;
			pass = (*(st++)-'0')%3;
		}else{
			*this = board();
		}
	}

	// Input: pos = the position of the update move (presented in bitboard)
	void update(unsigned long long pos){
		static std::pair<unsigned long long, unsigned long long>**** table = construct_update_table() ;

		if( pos ){
			unsigned long long *my_board, *op_board ;
			if( my_tile ){
				my_board = &white ;
				op_board = &black ;
			}
			else {
				my_board = &black ;
				op_board = &white ;
			}

			unsigned long long origin_my = *my_board ;
			unsigned long long origin_op = *op_board ;
			*my_board |= pos ;
			*op_board &= ~pos ;

	/*		unsigned long long p ;
			unsigned char yp, ypos ;
			unsigned long long tmp_my_board, tmp_op_board ;
			ypos = 1<<(__builtin_ctzll(pos)%8) ;  // ypos = 1<<y

			// up
			p = pos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do{
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p>>8 ;
			}while( *op_board & p ) ;
			if( *my_board & p ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}

			// up right
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p>>7 ;
				yp = yp<<1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}

			// right
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p<<1 ;
				yp = yp<<1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}
	
			// down right
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p<<9 ;
				yp = yp<<1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}
			
			// down
			p = pos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p<<8 ;
			}while( *op_board & p ) ;
			if( *my_board & p ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}
			
			// down left
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p<<7 ;
				yp = yp>>1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}			
			
			// left
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p>>1 ;
				yp = yp>>1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}
			
			// up left
			p = pos ;
			yp = ypos ;
			tmp_my_board = *my_board ;
			tmp_op_board = *op_board ;
			do {
				tmp_my_board |= p ;
				tmp_op_board &= ~p ;
				p = p>>9 ;
				yp = yp>>1 ;
			}while( *op_board & p && yp ) ;
			if( *my_board & p && yp ){
				*my_board = tmp_my_board ;
				*op_board = tmp_op_board ;
			}
		*/
			int x = __builtin_ctzll(pos)/8 ;
			int y = __builtin_ctzll(pos)%8 ;
		
			const unsigned long long row_mask = 	0xFF ;
			const unsigned long long col_mask = 	0x0101010101010101 ;
			const unsigned long long mplus_mask = 	0x0102040810204080 ;
			const unsigned long long mminus_mask = 	0x8040201008040201 ;
			static unsigned long long col_ones[] = {0, 0x0101010101010101, 
				0x0303030303030303, 0x0707070707070707, 0x0F0F0F0F0F0F0F0F,
				0x1F1F1F1F1F1F1F1F, 0x3F3F3F3F3F3F3F3F, 0x7F7F7F7F7F7F7F7F,
				0xFFFFFFFFFFFFFFFF } ;

			const unsigned long long col_magic = mplus_mask ;
			const unsigned long long m_magic = col_mask ;
	
			// Update row
			int shift = x<<3 ;
			unsigned long long my = (origin_my>>shift)&row_mask ;
			unsigned long long op = (origin_op>>shift)&row_mask ;

			unsigned long long update_my = table[0][my][op][y].first ;
			unsigned long long update_op = table[0][my][op][y].second ;
			
			*my_board = *my_board & ~(row_mask<<shift) ;
			*my_board = *my_board | (update_my<<shift) ;
			*op_board = *op_board & ~(row_mask<<shift) ;
			*op_board = *op_board | (update_op<<shift) ;

			// Update column
			my = origin_my>>y ;
			my = my & col_mask ;
			my = my * col_magic ;
			my = my>>56 ;
			my = my & row_mask ;

			op = origin_op>>y ;
			op = op & col_mask ;
			op = op * col_magic ;
			op = op>>56 ;
			op = op & row_mask ;

			update_my = table[1][my][op][x].first ;
			update_op = table[1][my][op][x].second ;
		
			*my_board = *my_board & ~(col_mask<<y) ;
			*my_board = *my_board | (update_my<<y) ;
			*op_board = *op_board & ~(col_mask<<y) ;
			*op_board = *op_board | (update_op<<y) ;
			
			// Update slope where m > 0
			if( x + y > 7 ){
				shift = (x+y-7)<<3 ; // (x+y-8)*8

				my = origin_my>>shift ;
				my = my & mplus_mask ;
				my = my * m_magic ;
				my = my>>56 ;
				my = my & row_mask ;

				op = origin_op>>shift ;
				op = op & mplus_mask ;
				op = op * m_magic ;
				op = op>>56 ;
				op = op & row_mask ;

				update_my = table[2][my][op][y].first ;
				update_op = table[2][my][op][y].second ;
		
				*my_board = *my_board & ~(mplus_mask<<shift) ;
				*my_board = *my_board | (update_my<<shift) ;
				*op_board = *op_board & ~(mplus_mask<<shift) ;
				*op_board = *op_board | (update_op<<shift) ;
			}
			else {
				shift = 7-x-y ; 

				my = origin_my<<shift ;
				my = my & mplus_mask ;
				my = my * m_magic ;
				my = my>>56 ;
				my = my & ~col_ones[shift] ;
				my = my & row_mask ;

				op = origin_op<<shift ;
				op = op & mplus_mask ;
				op = op * m_magic ;
				op = op>>56 ;
				op = op & ~col_ones[shift] ;
				op = op & row_mask ;

				update_my = table[2][my][op][y+shift].first ;
				update_op = table[2][my][op][y+shift].second ;
		
				*my_board = *my_board & ~((mplus_mask & ~col_ones[shift])>>shift) ;
				*my_board = *my_board | (update_my>>shift) ;
				*op_board = *op_board & ~((mplus_mask & ~col_ones[shift])>>shift) ;
				*op_board = *op_board | (update_op>>shift) ;
			}
		
			// Update slope where m < 0
			if( x < y ){
				shift = (y-x)<<3 ; // (y-x)*8

				my = origin_my<<shift ;
				my = my & mminus_mask ;
				my = my * m_magic ;
				my = my>>56 ;
				my = my & row_mask ;

				op = origin_op<<shift ;
				op = op & mminus_mask ;
				op = op * m_magic ;
				op = op>>56 ;
				op = op & row_mask ;

				update_my = table[3][my][op][y].first ;
				update_op = table[3][my][op][y].second ;
		
				*my_board = *my_board & ~(mminus_mask>>shift) ;
				*my_board = *my_board | (update_my>>shift) ;
				*op_board = *op_board & ~(mminus_mask>>shift) ;
				*op_board = *op_board | (update_op>>shift) ;
			}
			else {
				shift = x-y ; 

				my = origin_my<<shift ;
				my = my & mminus_mask ;
				my = my * m_magic ;
				my = my>>56 ;
				my = my & ~col_ones[shift] ;
				my = my & row_mask ;

				op = origin_op<<shift ;
				op = op & mminus_mask ;
				op = op * m_magic ;
				op = op>>56 ;
				op = op & ~col_ones[shift] ;
				op = op & row_mask ;

				update_my = table[3][my][op][y+shift].first ;
				update_op = table[3][my][op][y+shift].second ;
		
				*my_board = *my_board & ~((mminus_mask & ~col_ones[shift])>>shift) ;
				*my_board = *my_board | (update_my>>shift) ;
				*op_board = *op_board & ~((mminus_mask & ~col_ones[shift])>>shift) ;
				*op_board = *op_board | (update_op>>shift) ;
			}
			
			pass = 0;
		}
		else {
			++pass;
		}

		// player swap
		bool tmp_tile = my_tile ;
		my_tile = op_tile ;
		op_tile = tmp_tile ;
	}

	// Input: x = x-axis of the move 
	// 	  y = y-axis of the move
	void update(int x,int y){
		if( x == 8 && y == 0 )
			update(0) ;
		else
			update(1ULL<<(x*8+y)) ;
	}

	std::pair<unsigned long long, unsigned long long>**** construct_update_table(){
		std::pair<unsigned long long, unsigned long long>**** table = new std::pair<unsigned long long,unsigned long long>***[4] ;
		for(int i = 0 ; i < 4 ; i++){
			table[i] = new std::pair<unsigned long long,unsigned long long>**[256] ;
			for(int j = 0 ; j < 256 ; j++){
				table[i][j] = new std::pair<unsigned long long,unsigned long long>*[256] ;
				for(int k = 0 ; k < 256 ; k++)
					table[i][j][k] = new std::pair<unsigned long long,unsigned long long>[8] ;
			}
		}
		// table's structure: table[line][my][op][p]

		// Row's look-up table
		for(int my = 0 ; my < 256 ; ++my){
			for(int op = 0 ; op < 256 ; ++op){
				unsigned long long pos = 1 ;
				for(int p = 0 ; p < 8 ; ++p, pos = pos<<1){

					table[0][my][op][p] = std::pair<unsigned long long,unsigned long long>(my|pos,op&~pos) ;

					if( (my&op) || (my&pos) || (op&pos) )
						continue ;

					// right
					unsigned long long tmppos = pos<<1 ;
					unsigned char y = pos<<1 ;
					unsigned long long tmp_my = table[0][my][op][p].first ;
					unsigned long long tmp_op = table[0][my][op][p].second ;
					while( op & tmppos & y ){
						tmp_my |= tmppos ;
						tmp_op &= ~tmppos ;
						tmppos = tmppos<<1 ;
						y = y<<1 ;
					}
					if( my & tmppos & y ){
						table[0][my][op][p].first = tmp_my ;
						table[0][my][op][p].second = tmp_op ;
					}
		
					// left
					tmppos = pos>>1 ;
					y = pos>>1 ;
					tmp_my = table[0][my][op][p].first ;
					tmp_op = table[0][my][op][p].second ;
					while( op & tmppos & y ){
						tmp_my |= tmppos ;
						tmp_op &= ~tmppos ;
						tmppos = tmppos>>1 ;
						y = y>>1 ;
					}
					if( my & tmppos & y ){
						table[0][my][op][p].first = tmp_my ;
						table[0][my][op][p].second = tmp_op ;
					}
				}
			}
		}

		// Column's look-up table
		for(int my = 0 ; my < 256 ; ++my){
			for(int op = 0 ; op < 256 ; ++op){
				for(int p = 0 ; p < 8 ; ++p){
					unsigned long long row_my = table[0][my][op][p].first ;
					unsigned long long row_op = table[0][my][op][p].second ;
					unsigned long long tmp_my = 0 ;
					unsigned long long tmp_op = 0 ;
					const unsigned long long mask = 1 ;

					for(int y = 0 ; y < 8 ; y++){
						tmp_my |= (((row_my>>y)&mask)<<(y<<3)) ;
						tmp_op |= (((row_op>>y)&mask)<<(y<<3)) ;
					}

					table[1][my][op][p] = std::pair<unsigned long long,unsigned long long>(tmp_my,tmp_op) ;
				}
			}
		}		

		// Mpuls's look-up table
		for(int my = 0 ; my < 256 ; ++my){
			for(int op = 0 ; op < 256 ; ++op){
				for(int p = 0 ; p < 8 ; ++p){
					unsigned long long row_my = table[0][my][op][p].first ;
					unsigned long long row_op = table[0][my][op][p].second ;
					unsigned long long tmp_my = 0 ;
					unsigned long long tmp_op = 0 ;
					const unsigned long long mask = 1 ;

					for(int y = 0 ; y < 8 ; y++){
						tmp_my |= (((row_my>>y)&mask)<<(((7-y)<<3)+y)) ;
						tmp_op |= (((row_op>>y)&mask)<<(((7-y)<<3)+y)) ;
					}

					table[2][my][op][p] = std::pair<unsigned long long,unsigned long long>(tmp_my,tmp_op) ;
				}
			}
		}		

		// Mminus's look-up table
		for(int my = 0 ; my < 256 ; ++my){
			for(int op = 0 ; op < 256 ; ++op){
				for(int p = 0 ; p < 8 ; ++p){
					unsigned long long row_my = table[0][my][op][p].first ;
					unsigned long long row_op = table[0][my][op][p].second ;
					unsigned long long tmp_my = 0 ;
					unsigned long long tmp_op = 0 ;
					const unsigned long long mask = 1 ;

					for(int y = 0 ; y < 8 ; y++){
						tmp_my |= (((row_my>>y)&mask)<<(y*9)) ;
						tmp_op |= (((row_op>>y)&mask)<<(y*9)) ;
					}

					table[3][my][op][p] = std::pair<unsigned long long,unsigned long long>(tmp_my,tmp_op) ;
				}
			}
		}		

		return table ;
	}

	// Input: b = black board
	// 	  w = white board
	// 	  _pass = whether this time is pass or not
	void undo(const unsigned long long b, const unsigned long long w, int _pass){
		black = b ;
		white = w ;
		pass = _pass ;
		
		// player swap
		bool tmp_tile = my_tile ;
		my_tile = op_tile ;
		op_tile = tmp_tile ;
	}

	bool is_valid_move_new(const int x, const int y){
		static bool*** table = construct_valid_move_table() ;
		static unsigned long long ones[] = {0, 1, 3, 7, 15, 31, 63, 127, 255} ;
		
		unsigned long long my_board, op_board ;
		if( my_tile ){
			my_board = white ;
			op_board = black ;
		}
		else {
			my_board = black ;
			op_board = white ;
		}

		const unsigned long long row_mask = 	0xFF ;
		const unsigned long long col_mask = 	0x0101010101010101 ;
		const unsigned long long mplus_mask = 	0x0102040810204080 ;
		const unsigned long long mminus_mask = 	0x8040201008040201 ;

		const unsigned long long col_magic = mplus_mask ;
		const unsigned long long m_magic = col_mask ;
		
		// Check row
		int shift = x<<3 ; // x*8
		unsigned long long my = (my_board>>shift)&row_mask ;
		unsigned long long op = (op_board>>shift)&row_mask ;
		if( table[my][op][y] )
			return true ;
		
		// Check column
		my = my_board>>y ;
		my = my & col_mask ;
		my = my * col_magic ;
		my = my>>56 ;
		my = my & row_mask ;

		op = op_board>>y ;
		op = op & col_mask ;
		op = op * col_magic ;
		op = op>>56 ;
		op = op & row_mask ;

		if( table[my][op][x] )
			return true ;
		
		// Check slope where m > 0
		if( x + y > 7 ){
			shift = (x+y-7)<<3 ; // (x+y-8)*8

			my = my_board>>shift ;
			my = my & mplus_mask ;
			my = my * m_magic ;
			my = my>>56 ;
			my = my & row_mask ;

			op = op_board>>shift ;
			op = op & mplus_mask ;
			op = op * m_magic ;
			op = op>>56 ;
			op = op & row_mask ;

			if( table[my][op][y] )
				return true ;
		}
		else {
			shift = 7-x-y ; 

			my = my_board<<shift ;
			my = my & mplus_mask ;
			my = my * m_magic ;
			my = my>>56 ;
			my = my & ~ones[shift] ;
			my = my & row_mask ;

			op = op_board<<shift ;
			op = op & mplus_mask ;
			op = op * m_magic ;
			op = op>>56 ;
			op = op & ~ones[shift] ;
			op = op & row_mask ;

			if( table[my][op][y+shift] )
				return true ;
		}
		
		// Check slope where m < 0
		if( x < y ){
			shift = (y-x)<<3 ; // (y-x)*8

			my = my_board<<shift ;
			my = my & mminus_mask ;
			my = my * m_magic ;
			my = my>>56 ;
			my = my & row_mask ;

			op = op_board<<shift ;
			op = op & mminus_mask ;
			op = op * m_magic ;
			op = op>>56 ;
			op = op & row_mask ;

			if( table[my][op][y] )
				return true ;
		}
		else {
			shift = x-y ; 

			my = my_board<<shift ;
			my = my & mminus_mask ;
			my = my * m_magic ;
			my = my>>56 ;
			my = my & ~ones[shift] ;
			my = my & row_mask ;

			op = op_board<<shift ;
			op = op & mminus_mask ;
			op = op * m_magic ;
			op = op>>56 ;
			op = op & ~ones[shift] ;
			op = op & row_mask ;

			if( table[my][op][y+shift] )
				return true ;
		}
		return false ;
	}

	bool*** construct_valid_move_table(){
		bool*** table = new bool**[256] ;
		for(int i = 0 ; i < 256 ; i++){
			table[i] = new bool*[256] ;
			for(int j = 0 ; j < 256 ; j++)
				table[i][j] = new bool[8] ;
		}
		// table's structure: table[my][op][p]

		for(int my = 0 ; my < 256 ; ++my){
			for(int op = 0 ; op < 256 ; ++op){
				unsigned char pos = 1 ;
				for(int p = 0 ; p < 8 ; ++p, pos = pos<<1){

					table[my][op][p] = false ;

					if( (my&op) || (my&pos) || (op&pos) )
						continue ;

					// right
					unsigned char tmppos = pos<<1 ;
					if( op & tmppos ){
						do {
							tmppos = tmppos<<1 ;
						}while( op & tmppos ) ;
						if( my & tmppos ){
							table[my][op][p] = true ;
							continue ;
						}
					}
		
					// left
					tmppos = pos>>1 ;
					if( op & tmppos ){
						do {
							tmppos = tmppos>>1 ;
						}while( op & tmppos ) ;
						if( my & tmppos ){
							table[my][op][p] = true ;
							continue ;
						}
					}
				}
			}
		}

		return table ;
	}
	
	// Input: pos = the position of the move (presented in bitboard)
	// 	  ypos = the y-axis of the move (presented in bitboard)
	bool is_valid_move(const unsigned long long pos, const unsigned char ypos){

		if( black & pos || white & pos )
			return false ;

		if( pos ){
			unsigned long long my_board, op_board ;
			if( my_tile ){
				my_board = white ;
				op_board = black ;
			}
			else {
				my_board = black ;
				op_board = white ;
			}
			my_board |= pos ;
			op_board &= ~pos ;

	//		unsigned long long p ;
	//		unsigned char yp ;

			// up
//			p = pos>>8 ;
//			if( op_board & p ){
//				do{
//					p = p>>8 ;
//				}while( op_board & p ) ;
//				if( my_board & p )
//					return true ;
//			}

			// up right
//			p = pos>>7 ;
//			yp = ypos<<1 ;
//			if( op_board & p && yp ){
//				do {
//					p = p>>7 ;
//					yp = yp<<1 ;
//				}while( op_board & p && yp ) ;
//				if( my_board & p && yp )
//					return true ;
//			}

			// right
//			p = pos<<1 ;
//			yp = ypos<<1 ;
//			if( op_board & p && yp ){
//				do {
//					p = p<<1 ;
//					yp = yp<<1 ;
//				}while( op_board & p && yp ) ;
//				if( my_board & p && yp )
//					return true ;
//			}
	
			// down right
	//		p = pos<<9 ;
	//		yp = ypos<<1 ;
	//		if( op_board & p && yp ){
	//			do {
	//				p = p<<9 ;
	//				yp = yp<<1 ;
	//			}while( op_board & p && yp ) ;
	//			if( my_board & p && yp )
	//				return true ;
	//		}
			
			// down
//			p = pos<<8 ;
//			if( op_board & p ){
//				do {
//					p = p<<8 ;
//				}while( op_board & p ) ;
//				if( my_board & p )
//					return true ;
//			}
			
			// down left
//			p = pos<<7 ;
//			yp = ypos>>1 ;
//			if( op_board & p && yp ){
//				do {
//					p = p<<7 ;
//					yp = yp>>1 ;
//				}while( op_board & p && yp ) ;
//				if( my_board & p && yp )
//					return true ;
//			}
			
			// left
//			p = pos>>1 ;
//			yp = ypos>>1 ;
//			if( op_board & p && yp ){
//				do {
//					p = p>>1 ;
//					yp = yp>>1 ;
//				}while( op_board & p && yp ) ;
//				if( my_board & p && yp )
//					return true ;
//			}
			
	//		// up left
	//		p = pos>>9 ;
	//		yp = ypos>>1 ;
	//		if( op_board & p && yp ){
	//			do {
	//				p = p>>9 ;
	//				yp = yp>>1 ;
	//			}while( op_board & p && yp ) ;
	//			if( my_board & p && yp )
	//				return true ;
	//		}

			int ipos = __builtin_ctzll(pos) ;
			int ixpos = ipos/8 ;
			int iypos = ipos%8 ;
			if( is_valid_move_new(ixpos, iypos) )
				return true ;
		}

		return false ;
	}

	// Input: x = x-axis of the move 
	// 	  y = y-axis of the move
	bool is_valid_move_int(int x,int y){
		if( x == 8 && y == 0 ){ 
			unsigned long long b[64] ;
			return b == get_valid_move(b) ;
		}
		else {
			unsigned long long pos = 1ULL<<(x*8+y) ;
			unsigned char ypos = 1<<y ;
			return is_valid_move(pos, ypos) ;
		}
	}

	// Input: val = the pointer of valid move array
	unsigned long long* get_valid_move(unsigned long long* val){

		unsigned long long p = 1 ;
		unsigned char yp = 1 ;
		do{
			if( (~black & p || ~white & p) && is_valid_move(p, yp) ){
				*val = p ;
				++val ;
			}

			p = p<<1 ;
			yp = yp<<1 ;
			if(!yp)
				yp = 1 ;
		}while(p) ;

		return val ;
	}

	// Input: val = the pointer of valid move array
	int* get_valid_move(int* val){

		unsigned long long p = 1 ;
		unsigned char yp = 1 ;
		do{
			if( (~black & p || ~white & p) && is_valid_move(p, yp) ){
				*val = __builtin_ctzll(p) ;
				++val ;
			}

			p = p<<1 ;
			yp = yp<<1 ;
			if(!yp)
				yp = 1 ;
		}while(p) ;

		return val ;
	}

	void show_board(FILE*fp)const{
		static constexpr char c[]{'.','X','O'};
		if(my_tile)
        		fprintf(fp,"O's turn\n") ;
		else
        		fprintf(fp,"X's turn\n") ;

		fprintf(fp,"| ");
		for(int j=0;j!=8;++j){
			fprintf(fp,"|%c",'0'+j);
		}
		fprintf(fp,"|\n");

		unsigned long long p = 1 ;
		for(int i=0;i!=8;++i){
			fprintf(fp,"|%c",'0'+i);
			for(int j=0;j!=8;++j){
				if( black & p )
					fprintf(fp,"|%c",c[1]);
				else if( white & p )
					fprintf(fp,"|%c",c[2]);
				else
					fprintf(fp,"|%c",c[0]);
				p = p<<1 ;
			}
			fprintf(fp,"|\n");
		}
		fflush(fp);
	}

	std::pair<int,int> get_count()const{
		return std::pair<int,int>(__builtin_popcountll(black), __builtin_popcountll(white));
	}

	int get_score()const{
		return __builtin_popcountll(black) - __builtin_popcountll(white) ;
	}

	const bool operator[](int x)const{
		if( black & (1ULL<<x) )
			return BLACK ;
		else
			return WHITE ;
	}

	unsigned long long get_black()const{
		return black ;
	}

	unsigned long long get_white()const{
		return white ;
	}

	int get_pass()const{
		return pass;
	}

	bool get_my_tile()const{
		return my_tile;
	}

	bool is_game_over()const{
		return pass==2;
	}
};
#endif
