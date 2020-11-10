resource "aws_dynamodb_table" "music-table" {
  name           = "Music"
  read_capacity  = 20
  write_capacity = 20
  hash_key       = "Artist"
 attribute = {
    name = "Artist"
    type = "S"
  }
 ttl {
    attribute_name = "TimeToExist"
    enabled = false
  }
  tags {
    Name        = "dynamodb-music-test"
    Environment = "preprod"
  }
}
resource "aws_dynamodb_table_item" "music-table" {
  table_name = "${aws_dynamodb_table.music-table.name}"
  hash_key = "${aws_dynamodb_table.music-table.hash_key}"
  item = <<ITEM
  {
    "Artist": {
		"S": "sam cooke"
		},
    "Song": {
		"S": "danny boy"
		},
    "Year": {
		"N": "1959"
		}
  }
ITEM
}
resource "aws_dynamodb_table_item" "music-table-2" {
  table_name = "${aws_dynamodb_table.music-table.name}"
  hash_key = "${aws_dynamodb_table.music-table.hash_key}"
  item = <<ITEM
  {
    "Artist": {
		"S": "jackie wilson"
		},
    "Song": {
		"S": "reet petite"
		},
    "Year": {
		"N": "1958"
		}
  }
ITEM
}
resource "aws_dynamodb_table_item" "music-table-3" {
  table_name = "${aws_dynamodb_table.music-table.name}"
  hash_key = "${aws_dynamodb_table.music-table.hash_key}"
  item = <<ITEM
  {
    "Artist": {
		"S": "ben e king"
		},
    "Song": {
		"S": "stand by me"
		},
    "Year": {
		"N": "1961"
		}
  }
ITEM
}
