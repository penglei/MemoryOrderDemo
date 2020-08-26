import XCTest
@testable import Mokit

final class mokitTests: XCTestCase {
    func testExample() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct
        // results.
        XCTAssertEqual(MokitAttr().text, "Hello, World!")
    }

    static var allTests = [
        ("testExample", testExample),
    ]
}
