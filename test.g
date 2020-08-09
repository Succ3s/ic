#package main

#import fmt "std:fmt"

//
//
// comptime-ish
//
//

proc test_comptime_ish(const f: const T, const arr: []T) -> union{uint} {
	#for i in arr {
		#if i == f {
			break:find i;
		}
	}
	null
}






//
//
// null safety
//
//

// null safety
	// - ptrs, anyptr    // they are pointers
	// - cstr            // cstr is a strong alias to *u8
	// - proc            // proc pointer
	// - slice, str, any // null if the inner pointer is null
	// - enum, union     // except @NoNull
alias Foo struct {
	x: struct {
		y: struct { z: !*i32 },
	},
	b: bool
}

proc test_null_safety() {
	var v = 1;
	var _1: Foo;                   // error
	var _2 = Foo { b = true     }; // error
	var _3 = Foo { x.y.z = null }; // error
	var _4 = Foo { x.y.z = v.&  }; // ok
}







//
//
// control flow
//
//

proc test_control_flow(x: X) -> bool {
	var d: union{int, bool} = true;
	var res = switch d {
		x: int if x == 1 { true  }
		x: bool          { x     }
		else             { false }
	};

	switch 1 {
		2, 3  { fmt.println("foo") }
		1..10 { fmt.println("aaa") }
	}

	if  x.(bool) != null                 { fmt.println("aaa"); }
	for i in 0..1                        { fmt.println(i);     }
	if  true { fmt.println("aaa") } else { fmt.println("bbb"); }
}


//
//
// reflection
//
//

#import t "core:type"
proc print_struct(s: any) {
	switch t.type_info(s.id) {
		x: t.Named  {
			fmt.printf("struct %s { ", x.name);
			var x = x.inner.(t.Struct); // union cast returns union{T}
			assert(x != null); // union{T} can be compared to null
			// x is T here
			print_fields(x);
			fmt.println("}");
		}
		x: t.Struct {
			fmt.print("struct { ");
			print_fields(x);
			fmt.println("}");
		}
	}
}

proc print_fields(s: t.Struct) {
	assert(s.fields >= 1);
	fmt.print(s.names[0]);
	for i in 1..s.fields {
		fmt.print(", %s", s.names[i]);
	}
}



//
//
//	Trait-ish
//
//

type Arith(T: typeid) struct {
	add: proc(a, b: T) -> T;
	sub: proc(a, b: T) -> T;
	mul: proc(a, b: T) -> T;
	div: proc(a, b: T) -> T;
}



const i32Arith = Arith(i32) {
	add: proc(a, b: i32) -> i32 { a + b }
	sub: proc(a, b: i32) -> i32 { a - b }
	mul: proc(a, b: i32) -> i32 { a * b }
	div: proc(a, b: i32) -> i32 { a / b }
};

proc stuff(
	const arith: Arith(const T: typeid)),
	a, b: T
) -> T { arith.add(a, arith.mul(b, b)) }

proc test_trait_ish() { fmt.println(stuff(i32Arith, 2.-, 7)) }

//
//
//	Alias & Type
//
//

type  IntType  int;
alias IntAlias int;
proc test_types() {
	assert(IntType  != int);
	assert(IntAlias == int);
}

//
//
// UFCS-ish
//
//


proc neg(x: $T) -> T where t.is_numeric(T) { x.! }

proc test_ufcs_ish() {
	var a = 10;
	a->neg()
	 ->prinln();
}
