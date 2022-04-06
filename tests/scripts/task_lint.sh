#!/usr/bin/env bash
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

set -euxo pipefail

cleanup()
{
  rm -rf /tmp/$$.*
}
trap cleanup 0


echo "Convert scripts to Python..."
tests/scripts/task_convert_scripts_to_python.sh

# TODO: Remove this ad-hoc pip install once https://github.com/apache/tvm/pull/10741
# is added to the ci_lint Docker image
python3 -m pip install --user -r jenkins/requirements.txt
echo "Check Jenkinsfile generation"
python3 jenkins/generate.py --check

echo "Checking file types..."
python3 tests/lint/check_file_type.py

echo "Checking ASF license headers..."
tests/lint/check_asf_header.sh --local

echo "Linting the C++ code..."
tests/lint/cpplint.sh

echo "clang-format check..."
tests/lint/clang_format.sh

echo "Rust check..."
tests/lint/rust_format.sh

echo "black check..."
tests/lint/git-black.sh -i

echo "Linting the Python code..."
tests/lint/pylint.sh
tests/lint/flake8.sh

echo "Linting the JNI code..."
tests/lint/jnilint.sh

echo "Checking C++ documentation..."
tests/lint/cppdocs.sh

echo "Type checking with MyPy ..."
tests/scripts/task_mypy.sh
