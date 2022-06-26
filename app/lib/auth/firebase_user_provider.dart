import 'package:firebase_auth/firebase_auth.dart';
import 'package:rxdart/rxdart.dart';

class SmartLibraryFirebaseUser {
  SmartLibraryFirebaseUser(this.user);
  User user;
  bool get loggedIn => user != null;
}

SmartLibraryFirebaseUser currentUser;
bool get loggedIn => currentUser?.loggedIn ?? false;
Stream<SmartLibraryFirebaseUser> smartLibraryFirebaseUserStream() =>
    FirebaseAuth.instance
        .authStateChanges()
        .debounce((user) => user == null && !loggedIn
            ? TimerStream(true, const Duration(seconds: 1))
            : Stream.value(user))
        .map<SmartLibraryFirebaseUser>(
            (user) => currentUser = SmartLibraryFirebaseUser(user));
