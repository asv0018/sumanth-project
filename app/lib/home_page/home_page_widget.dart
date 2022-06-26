import '../flutter_flow/flutter_flow_icon_button.dart';
import '../flutter_flow/flutter_flow_theme.dart';
import '../flutter_flow/flutter_flow_util.dart';
import '../issue_new_book/issue_new_book_widget.dart';
import '../issued_books/issued_books_widget.dart';
import '../profile_page/profile_page_widget.dart';
import 'package:flutter/material.dart';
import 'package:flutter_spinkit/flutter_spinkit.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:cloud_firestore/cloud_firestore.dart';



class HomePageWidget extends StatefulWidget {
  const HomePageWidget({Key key}) : super(key: key);

  @override
  _HomePageWidgetState createState() => _HomePageWidgetState();
}

class _HomePageWidgetState extends State<HomePageWidget> {
  final scaffoldKey = GlobalKey<ScaffoldState>();
  FirebaseFirestore firestore = FirebaseFirestore.instance;
  var mybooksdata = [];
  var issuedBooks;
  @override
  void initState(){
    super.initState();
    WidgetsBinding.instance?.addPostFrameCallback((_) async {
      var data = await firestore.collection("users").doc(FirebaseAuth.instance.currentUser.uid).get();
      issuedBooks = data.data()["issuedBooks"];
      print(issuedBooks);
      for(var each in issuedBooks){
        mybooksdata.add(each);
      }
      var temp = [];
      for(var each in mybooksdata.reversed){
        temp.add(each);
      }
      mybooksdata.clear();
      mybooksdata = temp;
      setState(() {});
    });
    }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      key: scaffoldKey,
      backgroundColor: Color(0xFFEFF4F7),
      floatingActionButton: FloatingActionButton(
        onPressed: () async {
          await Navigator.push(
            context,
            MaterialPageRoute(
              builder: (context) => IssueNewBookWidget(),
            ),
          );
        },
        backgroundColor: Color(0xFF39A2DB),
        elevation: 10,
        child: FaIcon(
          FontAwesomeIcons.plus,
          color: Color(0xFFEFF4F7),
          size: 24,
        ),
      ),
      body: SafeArea(
        child: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.max,
            children: [
              Padding(
                padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 12),
                child: Container(
                  width: MediaQuery.of(context).size.width,
                  height: 70,
                  decoration: BoxDecoration(
                    color: Color(0xFFEFF4F7),
                    boxShadow: [
                      BoxShadow(
                        blurRadius: 5,
                        color: Color(0x32171717),
                        offset: Offset(0, 2),
                      )
                    ],
                    borderRadius: BorderRadius.circular(0),
                  ),
                  child: Padding(
                    padding: EdgeInsetsDirectional.fromSTEB(0, 0, 0, 12),
                    child: Column(
                      mainAxisSize: MainAxisSize.max,
                      children: [
                        Expanded(
                          child: Row(
                            mainAxisSize: MainAxisSize.max,
                            mainAxisAlignment: MainAxisAlignment.start,
                            children: [
                              Expanded(
                                child: Padding(
                                  padding: EdgeInsetsDirectional.fromSTEB(
                                      5, 20, 5, 0),
                                  child: Text(
                                    'Previously Issued Books',
                                    textAlign: TextAlign.start,
                                    style: FlutterFlowTheme.of(context)
                                        .subtitle1
                                        .override(
                                          fontFamily: 'Lexend Deca',
                                          color: Color(0xFF073944),
                                          fontSize: 26,
                                          fontWeight: FontWeight.bold,
                                        ),
                                  ),
                                ),
                              ),
                              Padding(
                                padding:
                                    EdgeInsetsDirectional.fromSTEB(0, 10, 0, 0),
                                child: FlutterFlowIconButton(
                                  borderColor: Colors.transparent,
                                  borderWidth: 1,
                                  buttonSize: 60,
                                  icon: Icon(
                                    Icons.account_circle,
                                    color: Color(0xFF073944),
                                    size: 40,
                                  ),
                                  onPressed: () async {
                                    await Navigator.push(
                                      context,
                                      MaterialPageRoute(
                                        builder: (context) =>
                                            ProfilePageWidget(),
                                      ),
                                    );
                                  },
                                ),
                              ),
                            ],
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
              SingleChildScrollView(
                child: ListView.builder(shrinkWrap: true,itemBuilder:  (BuildContext context, int index) {
                  return GestureDetector(
                          child: Column(
                            mainAxisSize: MainAxisSize.max,
                            children: [
                              Padding(
                                padding:
                                EdgeInsetsDirectional.fromSTEB(0, 10, 0, 8),
                                child: Container(
                                  width: MediaQuery.of(context).size.width * 0.85,
                                  height: 300,
                                  decoration: BoxDecoration(
                                    color: Color(0xFFEFF4F7),
                                    image: DecorationImage(
                                      fit: BoxFit.fill,
                                      image: Image.network(
                                        mybooksdata[index]["book-data"]["imgurl"],
                                      ).image,
                                    ),
                                    boxShadow: [
                                      BoxShadow(
                                        blurRadius: 3,
                                        color: Color(0xFF57636C),
                                        offset: Offset(0, 1),
                                      )
                                    ],
                                    borderRadius: BorderRadius.circular(8),
                                    border: Border.all(
                                      color: Color(0x0039A2DB),
                                      width: 1,
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                          onTap:() async {
                            await Navigator.push(
                              context,
                              MaterialPageRoute(
                                builder: (context) =>
                                    IssuedBooksWidget(issuedBooks: issuedBooks, data: mybooksdata[index],arr: (mybooksdata.length-1-index)),
                              ),
                            );
                          }
                  );
                }, itemCount: mybooksdata.length,),
              )
            ],
          ),
        ),
      ),
    );
  }
}
