import CWrapper

public struct MokitAttr {
    public var text = "Hello, World~~~!!"
    public init(){
    }
}


public func greet(person: String) -> String {
    let greeting = "Hello, " + person + "!"
    return greeting
}

public func CalcFactorial(n: Int) -> Int {
    return calc_factorial(Int32(n))
}

public func store_store_sample() {
    run_produce_consume_reorder_example()
}

public func load_store_sample() {
    run_mutual_exclusion_reorder_example()
}

public func run_reordering_samples() {
    load_store_sample()
}
