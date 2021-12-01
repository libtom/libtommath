import XCTest
import libtommath

/* ---> Basic Manipulations <--- */

extension mp_int {
    var isZero: Bool { used == 0 }
    var isNeg: Bool { sign == MP_NEG }
    var isEven: Bool { used == 0 || (dp[0] & 1) == 0 }
    var isOdd: Bool { !isEven }
}

func mp_get_u32(_ a: UnsafePointer<mp_int>) -> UInt32 {
    return UInt32(bitPattern: mp_get_i32(a))
}

class LibTommathTests: XCTestCase {

    override func setUpWithError() throws {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDownWithError() throws {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testTrivialStuff() throws {
        var a = mp_int()
        var b = mp_int()
        var c = mp_int()
        var d = mp_int()
        
        XCTAssertEqual(mp_init(&a), MP_OKAY)
        XCTAssertEqual(mp_init(&b), MP_OKAY)
        XCTAssertEqual(mp_init(&c), MP_OKAY)
        XCTAssertEqual(mp_init(&d), MP_OKAY)
        
        defer {
            mp_clear(&a)
            mp_clear(&b)
            mp_clear(&c)
            mp_clear(&d)
        }
        
        XCTAssert(mp_error_to_string(MP_OKAY) != nil)

        /* a: 0->5 */
        mp_set(&a, 5)
        /* a: 5-> b: -5 */
        XCTAssertEqual(mp_neg(&a, &b), MP_OKAY)
        XCTAssertEqual(mp_cmp(&a, &b), MP_GT)
        XCTAssertEqual(mp_cmp(&b, &a), MP_LT)
        XCTAssertTrue(b.isNeg)
        /* a: 5-> a: -5 */
        var t = a  // Fix compiler error: Overlapping accesses to 'a', but modification requires exclusive access; consider copying to a local variable
        XCTAssertEqual(mp_neg(&t, &a), MP_OKAY)
        XCTAssertEqual(mp_cmp(&b, &a), MP_EQ)
        XCTAssertTrue(a.isNeg)
        /* a: -5-> b: 5 */
        XCTAssertEqual(mp_abs(&a, &b), MP_OKAY)
        XCTAssertTrue(!b.isNeg)
        /* a: -5-> b: -4 */
        XCTAssertEqual(mp_add_d(&a, 1, &b), MP_OKAY)
        XCTAssertTrue(b.isNeg)
        XCTAssertEqual(mp_get_i32(&b), -4)
        XCTAssertEqual(mp_get_u32(&b), UInt32(bitPattern: -4))
        XCTAssertEqual(mp_get_mag_u32(&b), 4)
        /* a: -5-> b: 1 */
        XCTAssertEqual(mp_add_d(&a, 6, &b), MP_OKAY)
        XCTAssertEqual(mp_get_u32(&b), 1)
        /* a: -5-> a: 1 */
        t = a
        XCTAssertEqual(mp_add_d(&t, 6, &a), MP_OKAY)
        XCTAssertEqual(mp_get_u32(&a), 1)
        mp_zero(&a);
        /* a: 0-> a: 6 */
        t = a
        XCTAssertEqual(mp_add_d(&t, 6, &a), MP_OKAY)
        XCTAssertEqual(mp_get_u32(&a), 6)

        mp_set(&a, 42)
        mp_set(&b, 1)
        t = b
        XCTAssertEqual(mp_neg(&t, &b), MP_OKAY)
        mp_set(&c, 1)
        XCTAssertEqual(mp_exptmod(&a, &b, &c, &d), MP_OKAY)

        mp_set(&c, 7)
        /* same here */
        XCTAssertTrue(mp_exptmod(&a, &b, &c, &d) != MP_OKAY)

        XCTAssertTrue(a.isEven != a.isOdd)
    }
}
