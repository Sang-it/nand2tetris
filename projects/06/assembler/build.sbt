val scala3Version = "3.3.1"

lazy val root = project
  .in(file("."))
  .settings(
    name := "assembler",
    version := "0.1.0-SNAPSHOT",

    scalaVersion := scala3Version,

    libraryDependencies += "org.scalameta" %% "munit" % "0.7.29",
    libraryDependencies += "com.lihaoyi" %% "fastparse" % "3.0.2",
    libraryDependencies += "org.scalatest" %% "scalatest" % "3.2.18" % "test"
  )
