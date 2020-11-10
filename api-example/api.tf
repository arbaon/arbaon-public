resource "aws_api_gateway_rest_api" "dynodbops" {
  name        = "dynamodbapi"
  description = "Test APi"
}

resource "aws_api_gateway_resource" "dynamodbmanager" {
  rest_api_id = "${aws_api_gateway_rest_api.dynodbops.id}"
  parent_id   = "${aws_api_gateway_rest_api.dynodbops.root_resource_id}"
  path_part   = "test"
}

resource "aws_api_gateway_method" "postmethod" {
  rest_api_id   = "${aws_api_gateway_rest_api.dynodbops.id}"
  resource_id   = "${aws_api_gateway_resource.dynamodbmanager.id}"
  http_method   = "POST"
  authorization = "NONE"
}

resource "aws_api_gateway_integration" "lambda" {
  rest_api_id = "${aws_api_gateway_rest_api.dynodbops.id}"
  resource_id = "${aws_api_gateway_method.postmethod.resource_id}"
  http_method = "${aws_api_gateway_method.postmethod.http_method}"

  integration_http_method = "POST"
  type                    = "AWS"
  uri                     = "${aws_lambda_function.test_lambda.invoke_arn}"
}

resource "aws_api_gateway_method_response" "200" {
  rest_api_id = "${aws_api_gateway_rest_api.dynodbops.id}"
  resource_id = "${aws_api_gateway_resource.dynamodbmanager.id}"
  http_method = "${aws_api_gateway_method.postmethod.http_method}"
  status_code = "200"
  response_models = "${var.responsemodel}"
}

resource "aws_api_gateway_integration_response" "lambdaintegrationresponse" {
  rest_api_id = "${aws_api_gateway_rest_api.dynodbops.id}"
  resource_id = "${aws_api_gateway_resource.dynamodbmanager.id}"
  http_method = "${aws_api_gateway_method.postmethod.http_method}"
  status_code = "${aws_api_gateway_method_response.200.status_code}"
  response_templates = "${var.responsetemplates}"
  depends_on = ["aws_api_gateway_integration.lambda"]
}

resource "aws_api_gateway_deployment" "lambdadeployment" {
  depends_on = ["aws_api_gateway_integration.lambda"]

  rest_api_id = "${aws_api_gateway_rest_api.dynodbops.id}"
  stage_name  = "test"
}

resource "aws_lambda_permission" "lambda_permission" {
  action        = "lambda:InvokeFunction"
  function_name = "lambda_02"
  principal     = "apigateway.amazonaws.com"
  source_arn 	=  "${aws_api_gateway_rest_api.dynodbops.execution_arn}/*/*/*"
  depends_on = ["aws_api_gateway_deployment.lambdadeployment"]
}

output "endpoint" {
  value = "${aws_api_gateway_rest_api.dynodbops.id}.execute-api.eu-west-2.amazonaws.com/test/dynamodbmanager"
}
